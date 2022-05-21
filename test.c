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

	char *text = ": sq dup * ; 4 sq . cr";
	printf("%s\n\n", text);
	ForthProgram p = forth_compile(forth, text);

	forth_printProgram(p);
	printf("\n");

	//forth_run(forth, (ForthProgram){program, 100});
	forth_run(forth, p);

	forth_freeInstance(forth);
	return 0;
}
