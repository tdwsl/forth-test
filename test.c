#include "forth/instance.h"
#include "forth/machine.h"
#include "forth/compile.h"
#include <stdio.h>

int main() {
	ForthInstance *forth = forth_newInstance();

	ForthProgram p = forth_compileFile(forth, "test.fth");
	//forth_printProgram(p);
	forth_run(forth, p);
	forth_freeProgram(p);

	forth_freeInstance(forth);
	return 0;
}
