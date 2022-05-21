#include "forth/instance.h"
#include "forth/machine.h"
#include <stdio.h>
#include <stdlib.h>

int forth_chars2int(char *c) {
	int c0 = c[0], c1 = c[1], c2 = c[2], c3 = c[3];
	return (c0<<24)|(c1<<16)|(c2<<8)|c3;
}

void forth_int2chars(int i, char *c) {
	c[0] = (i>>24)&0x00FF;
	c[1] = (i>>16)&0x00FF;
	c[2] = (i>>8)&0x00FF;
	c[3] = i&0x00FF;
}

void forth_addInstruction(ForthProgram *p, char ins) {
	p->instructions = realloc(p->instructions, (++p->size));
	p->instructions[p->size-1] = ins;
}

void forth_addInteger(ForthProgram *p, int i) {
	char c[4];
	forth_int2chars(i, c);
	for(int j = 0; j < 4; j++)
		forth_addInstruction(p, c[j]);
}

int forth_instructionOperands(char ins) {
	switch(ins) {
	case FORTH_JUMP:
	case FORTH_JZ:
	case FORTH_JNZ:
	case FORTH_PUTSTR:
	case FORTH_PUSH:
	case FORTH_CALL:
		return 4;
	default:
		return 0;
	}
}

void forth_run(ForthInstance *forth, ForthProgram p) {
	int n1, n2;
	int pc = 0;
	char *program = p.instructions;
	while(pc < p.size)
		switch(program[pc++]) {
		case FORTH_CALL:
			forth->rstack[forth->rsp++] = pc+4;
			pc = forth_chars2int(program+pc);
			break;
		case FORTH_RETURN:
			pc = forth->rstack[--forth->rsp];
			break;
		case FORTH_JUMP:
			pc = forth_chars2int(program+pc);
			break;
		case FORTH_PLUS:
			n2 = forth_pop(forth);
			n1 = forth_pop(forth);
			forth_push(forth, n1+n2);
			break;
		case FORTH_MINUS:
			n2 = forth_pop(forth);
			n1 = forth_pop(forth);
			forth_push(forth, n1-n2);
			break;
		case FORTH_MUL:
			n2 = forth_pop(forth);
			n1 = forth_pop(forth);
			forth_push(forth, n1*n2);
			break;
		case FORTH_DIV:
			n2 = forth_pop(forth);
			n1 = forth_pop(forth);
			forth_push(forth, n1/n2);
			break;
		case FORTH_PERIOD:
			printf("%d ", forth_pop(forth));
			break;
		case FORTH_CR:
			printf("\n");
			break;
		case FORTH_PUTSTR:
			n1 = forth_chars2int(program+pc);
			printf("%s", forth->strings[n1]);
			pc += 4;
			break;
		case FORTH_END:
			return;
		case FORTH_DROP:
			forth_pop(forth);
			break;
		case FORTH_DUP:
			n1 = forth_pop(forth);
			forth_push(forth, n1);
			forth_push(forth, n1);
			break;
		case FORTH_PUSH:
			n1 = forth_chars2int(program+pc);
			forth_push(forth, n1);
			pc += 4;
			break;
		case FORTH_JNZ:
			n1 = forth_chars2int(program+pc);
			pc += 4;
			if(forth_pop(forth))
				pc = n1;
			break;
		case FORTH_JZ:
			n1 = forth_chars2int(program+pc);
			pc += 4;
			if(!forth_pop(forth))
				pc = n1;
			break;
		case FORTH_DEC:
			n1 = forth_pop(forth);
			forth_push(forth, --n1);
			break;
		case FORTH_INC:
			n1 = forth_pop(forth);
			forth_push(forth, ++n1);
			break;
		case FORTH_EQUAL:
			n2 = forth_pop(forth);
			n1 = forth_pop(forth);
			forth_push(forth, n1 == n2);
			break;
		case FORTH_GREATER:
			n2 = forth_pop(forth);
			n1 = forth_pop(forth);
			forth_push(forth, n1 > n2);
			break;
		case FORTH_LESS:
			n2 = forth_pop(forth);
			n1 = forth_pop(forth);
			forth_push(forth, n1 < n2);
			break;
		}
}

void forth_printProgram(ForthProgram p) {
	int pc = 0;
	while(pc < p.size) {
		printf("%d\t", pc);
		switch(p.instructions[pc++]) {
		case FORTH_PUSH:
			printf("push %d", forth_chars2int(p.instructions+pc));
			pc += 4;
			break;
		case FORTH_DROP:
			printf("drop");
			break;
		case FORTH_PLUS:
			printf("plus");
			break;
		case FORTH_MINUS:
			printf("minus");
			break;
		case FORTH_MUL:
			printf("mul");
			break;
		case FORTH_DIV:
			printf("div");
			break;
		case FORTH_CALL:
			printf("call #%d", forth_chars2int(p.instructions+pc));
			pc += 4;
			break;
		case FORTH_RETURN:
			printf("return");
			break;
		case FORTH_CR:
			printf("cr");
			break;
		case FORTH_PERIOD:
			printf("put");
			break;
		case FORTH_PUTSTR:
			printf("putstr #%d", forth_chars2int(
						p.instructions+pc));
			pc += 4;
			break;
		case FORTH_END:
			printf("end");
			break;
		case FORTH_JUMP:
			printf("jump #%d", forth_chars2int(
						p.instructions+pc));
			pc += 4;
			break;
		case FORTH_JNZ:
			printf("jnz #%d", forth_chars2int(
						p.instructions+pc));
			pc += 4;
			break;
		case FORTH_JZ:
			printf("jz #%d", forth_chars2int(
						p.instructions+pc));
			pc += 4;
			break;
		case FORTH_DUP:
			printf("dup");
			break;
		case FORTH_DEC:
			printf("dec");
			break;
		case FORTH_INC:
			printf("inc");
			break;
		case FORTH_EQUAL:
			printf("equal");
			break;
		case FORTH_GREATER:
			printf("greater");
			break;
		case FORTH_LESS:
			printf("less");
			break;
		}
		printf("\n");
	}
}
