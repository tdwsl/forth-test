#ifndef FORTH_INSTANCE_H
#define FORTH_INSTANCE_H

#define FORTH_STACK_SIZE 512
#define FORTH_RSTACK_SIZE 256
#define FORTH_LSTACK_SIZE 32

typedef struct forthInstance {
	int stack[FORTH_STACK_SIZE];
	int rstack[FORTH_RSTACK_SIZE];
	int lstack[FORTH_LSTACK_SIZE];
	int sp, rsp, lsp;
	int *addresses;
	char **strings;
} ForthInstance;

ForthInstance *forth_newInstance();
void forth_freeInstance(ForthInstance *forth);

void forth_push(ForthInstance *forth, int n);
int forth_pop(ForthInstance *forth);

int forth_addString(ForthInstance *forth, char *s);

#endif
