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
		else if(strcmp(s, "skip") == 0)
			forth_addInstruction(&p, FORTH_DROP);
		else if(strcmp(s, ".") == 0)
			forth_addInstruction(&p, FORTH_PERIOD);
		else if(strcmp(s, "cr") == 0)
			forth_addInstruction(&p, FORTH_CR);
		else if(strcmp(s, "dup") == 0)
			forth_addInstruction(&p, FORTH_DUP);

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

	for(int i = 0; i < p.size; i++)
		forth_addInstruction(&p0, p.instructions[i]);

	free(p.instructions);

	return p0;
}