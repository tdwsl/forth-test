#ifndef FORTH_COMPILE_H
#define FORTH_COMPILE_H

#include "forth/instance.h"
#include "forth/machine.h"

char **forth_splitString(char *text);
ForthProgram forth_compile(ForthInstance *forth, char *text);

#endif
