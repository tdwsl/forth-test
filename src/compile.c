#include "forth/compile.h"
#include "forth/instance.h"
#include "forth/machine.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

char **forth_splitString(char *text) {
	int sn = 0;
	char **strings = 0;

	const int buf = 20;
	int max = 50;
	char *s = malloc(max);
	int len = 0;

	bool comment = false;
	char quote = 0;

	for(char *c = text; ; c++) {
		if(*c == '\n' || *c == 0) {
			s[len] = 0;
			len = 0;

			if(!quote && strcmp(s, "\\") == 0) {
				quote = 0;
				comment = false;
				continue;
			}

			if(quote || s[0]) {
				sn++;
				strings = realloc(strings, sizeof(char*)*sn);
				strings[sn-1] = malloc(strlen(s)+1);
				strcpy(strings[sn-1], s);
			}

			quote = 0;
			comment = false;

			if(*c == 0)
				break;
			continue;
		}

		if(comment)
			continue;

		if(quote) {
			if(*c == quote) {
				s[len] = 0;
				len = 0;

				sn++;
				strings = realloc(strings, sizeof(char*)*sn);
				strings[sn-1] = malloc(strlen(s)+1);
				strcpy(strings[sn-1], s);

				quote = 0;
			}
			else {
				s[len++] = *c;
				if(len > max-buf) {
					max += buf;
					s = realloc(s, max);
				}
			}
			continue;
		}

		if(*c == ' ' || *c == '\t') {
			s[len] = 0;
			len = 0;

			if(strcmp(s, "\\") == 0) {
				comment = true;
				continue;
			}

			if(s[0]) {
				if(strcmp(s, ".\"") == 0
						|| strcmp(s, ".'") == 0
						|| strcmp(s, ".(") == 0) {
					quote = s[1];
					s[1] = '"';
				}

				sn++;
				strings = realloc(strings, sizeof(char*)*sn);
				strings[sn-1] = malloc(strlen(s)+1);
				strcpy(strings[sn-1], s);

			}

			continue;
		}

		if(*c >= 'A' && *c <= 'Z')
			s[len++] = *c - 'A' + 'a';
		else
			s[len++] = *c;
		if(len > max-buf) {
			max += buf;
			s = realloc(s, max);
		}
	}

	free(s);

	sn++;
	strings = realloc(strings, sizeof(char*)*sn);
	strings[sn-1] = 0;

	return strings;
}

ForthProgram forth_compile(ForthInstance *forth, char *text) {
	char **strings = forth_splitString(text);

	ForthProgram p0 = (ForthProgram){0, 0};
	ForthProgram p = (ForthProgram){0, 0};

	forth_addInstruction(&p0, FORTH_JUMP);
	forth_addInteger(&p0, 0);

	char **identifiers = 0;
	int *locations = 0;
	int num_identifiers = 0;

	ForthProgram op;
	bool def = false;

	int if_a[FORTH_RSTACK_SIZE];
	int else_a[FORTH_RSTACK_SIZE];
	int if_sp = 0;

	int current;

	for(int j = 0; strings[j]; j++) {
		char *s = strings[j];

		int n = 0;
		bool is_num = true;
		bool neg = false;
		for(int i = 0; s[i] && is_num; i++) {
			if(s[i] == '-') {
				if(i == 0 && s[i+1])
					neg = true;
				else
					is_num = false;
			}
			else if(s[i] >= '0' && s[i] <= '9')
				n = n * 10 + s[i] - '0';
			else
				is_num = false;
		}
		if(neg)
			n *= -1;

		if(is_num) {
			forth_addInstruction(&p, FORTH_PUSH);
			forth_addInteger(&p, n);
		}

		else if(strcmp(s, ":") == 0) {
			if(def) {
				printf("nested : !\n");
				continue;
			}

			num_identifiers++;
			identifiers = realloc(identifiers, sizeof(char*)
					* num_identifiers);
			/*s = strings[++j];
			identifiers[num_identifiers-1] = malloc(strlen(s)+1);
			strcpy(identifiers[num_identifiers-1], s);*/
			identifiers[num_identifiers-1] = strings[++j];

			locations = realloc(locations, sizeof(int)
					* num_identifiers);
			locations[num_identifiers-1] = p0.size;
			current = p0.size;

			op = p;
			def = true;
			p = p0;
		}
		else if(strcmp(s, ";") == 0) {
			if(!def)
				continue;

			forth_addInstruction(&p, FORTH_RETURN);

			p0 = p;
			p = op;
			def = false;
		}

		else if(strcmp(s, "if") == 0) {
			forth_addInstruction(&p, FORTH_JZ);
			else_a[if_sp] = -1;
			if_a[if_sp++] = p.size;
			forth_addInteger(&p, 0);
		}
		else if(strcmp(s, "else") == 0) {
			forth_addInstruction(&p, FORTH_JUMP);
			else_a[if_sp-1] = p.size;
			forth_addInteger(&p, 0);
		}
		else if(strcmp(s, "then") == 0) {
			if_sp--;
			if(else_a[if_sp] != -1) {
				forth_int2chars(else_a[if_sp]+4,
						p.instructions+if_a[if_sp]);
				forth_int2chars(p.size,
						p.instructions
						+else_a[if_sp]);
			}
			else
				forth_int2chars(p.size,
						p.instructions+if_a[if_sp]);
		}

		else if(strcmp(s, "recurse") == 0) {
			if(!def)
				continue;
			forth_addInstruction(&p, FORTH_CALL);
			forth_addInteger(&p, current);
		}

		else if(strcmp(s, ".\"") == 0) {
			s = strings[++j];
			int i = forth_addString(forth, s);
			forth_addInstruction(&p, FORTH_PUTSTR);
			forth_addInteger(&p, i);
		}

		else if(strcmp(s, "+") == 0)
			forth_addInstruction(&p, FORTH_PLUS);
		else if(strcmp(s, "-") == 0)
			forth_addInstruction(&p, FORTH_MINUS);
		else if(strcmp(s, "/") == 0)
			forth_addInstruction(&p, FORTH_DIV);
		else if(strcmp(s, "*") == 0)
			forth_addInstruction(&p, FORTH_MUL);
		else if(strcmp(s, "drop") == 0)
			forth_addInstruction(&p, FORTH_DROP);
		else if(strcmp(s, ".") == 0)
			forth_addInstruction(&p, FORTH_PERIOD);
		else if(strcmp(s, "cr") == 0)
			forth_addInstruction(&p, FORTH_CR);
		else if(strcmp(s, "dup") == 0)
			forth_addInstruction(&p, FORTH_DUP);
		else if(strcmp(s, "=") == 0)
			forth_addInstruction(&p, FORTH_EQUAL);
		else if(strcmp(s, ">") == 0)
			forth_addInstruction(&p, FORTH_GREATER);
		else if(strcmp(s, "<") == 0)
			forth_addInstruction(&p, FORTH_LESS);
		else if(strcmp(s, ">=") == 0) {
			forth_addInstruction(&p, FORTH_INC);
			forth_addInstruction(&p, FORTH_GREATER);
		}
		else if(strcmp(s, "<=") == 0) {
			forth_addInstruction(&p, FORTH_DEC);
			forth_addInstruction(&p, FORTH_LESS);
		}
		else if(strcmp(s, "swap") == 0)
			forth_addInstruction(&p, FORTH_SWAP);
		else if(strcmp(s, "over") == 0)
			forth_addInstruction(&p, FORTH_OVER);
		else if(strcmp(s, "rot") == 0)
			forth_addInstruction(&p, FORTH_ROT);
		else if(strcmp(s, "depth") == 0)
			forth_addInstruction(&p, FORTH_DEPTH);

		else {
			int wd = -1;
			for(int i = 0; wd == -1 && i < num_identifiers; i++)
				if(strcmp(identifiers[i], s) == 0)
					wd = i;
			if(wd == -1) {
				printf("%s ?\n", s);
				continue;
			}

			int a = locations[wd];
			forth_addInstruction(&p, FORTH_CALL);
			forth_addInteger(&p, a);
		}
	}

	if(identifiers) {
		free(identifiers);
		free(locations);
	}
	for(int i = 0; strings[i]; i++)
		free(strings[i]);
	free(strings);

	forth_int2chars(p0.size, p0.instructions+1);

	for(int i = 0; i < p.size; i++) {
		int n;
		switch(p.instructions[i]) {
		case FORTH_JUMP:
		case FORTH_JZ:
		case FORTH_JNZ:
			n = forth_chars2int(p.instructions+i+1);
			n += p0.size;
			forth_int2chars(n, p.instructions+i+1);
			break;
		default:
			break;
		}
		i += forth_instructionOperands(p.instructions[i]);
	}

	for(int i = 0; i < p.size; i++)
		forth_addInstruction(&p0, p.instructions[i]);

	free(p.instructions);

	return p0;
}

ForthProgram forth_compileFile(ForthInstance *forth, const char *filename) {
	FILE *fp = fopen(filename, "r");
	if(!fp) {
		printf("failed to open %s\n", filename);
		forth_freeInstance(forth);
		exit(1);
	}

	const int buf = 20;
	int max = 300;
	char *s = malloc(max);
	int len = 0;

	for(char c = fgetc(fp); c != EOF; c = fgetc(fp)) {
		s[len++] = c;
		if(len > max-buf) {
			max += buf;
			s = realloc(s, buf);
		}
	}
	fclose(fp);
	s[len] = 0;

	ForthProgram p = forth_compile(forth, s);
	free(s);

	return p;
}
