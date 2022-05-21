#ifndef FORTH_MACHINE_H
#define FORTH_MACHINE_H

#include "forth/instance.h"
#include <stdlib.h>

enum {
	FORTH_PUSH,
	FORTH_DROP,
	FORTH_CALL,
	FORTH_MUL,
	FORTH_DIV,
	FORTH_PLUS,
	FORTH_MINUS,
	FORTH_DUP,
	FORTH_PERIOD,
	FORTH_CR,
	FORTH_JUMP,
	FORTH_RETURN,
	FORTH_PUTSTR,
	FORTH_END,
	FORTH_JZ,
	FORTH_JNZ,
	FORTH_INC,
	FORTH_DEC,
	FORTH_EQUAL,
	FORTH_GREATER,
	FORTH_LESS,
};

typedef struct forthProgram {
	char *instructions;
	int size;
} ForthProgram;

void forth_int2chars(int i, char *c);
int forth_chars2int(char *c);

void forth_run(ForthInstance *forth, ForthProgram p);
void forth_printProgram(ForthProgram p);
int forth_instructionOperands(char ins);

void forth_addInstruction(ForthProgram *p, char ins);
void forth_addInteger(ForthProgram *p, int i);

#define forth_freeProgram(P) { if(P.instructions) free(P.instructions); }

#endif
