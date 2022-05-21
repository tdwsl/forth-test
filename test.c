#include "forth/instance.h"
#include "forth/machine.h"
#include "forth/compile.h"
#include <stdio.h>

int main() {
	char program[100] = {
		FORTH_JUMP,0,0,0,8,

		FORTH_DUP,
		FORTH_MUL,
		FORTH_RETURN,

		FORTH_PUSH,0,0,0,1,
		FORTH_PUSH,0,0,0,2,
		FORTH_PLUS,
		FORTH_PERIOD,
		FORTH_CR,

		FORTH_PUSH,0,0,0,4,
		FORTH_CALL,0,0,0,5,
		FORTH_PERIOD,
		FORTH_CR,

		FORTH_END,
	};

	ForthInstance *forth = forth_newInstance();

	ForthProgram p = forth_compileFile(forth, "test.fth");
	//forth_printProgram(p);
	forth_run(forth, p);
	forth_freeProgram(p);

	forth_freeInstance(forth);
	return 0;
}
