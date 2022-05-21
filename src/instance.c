#include "forth/instance.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ForthInstance *forth_newInstance() {
	ForthInstance *forth = malloc(sizeof(ForthInstance));

	forth->addresses = 0;

	forth->sp = 0;
	forth->rsp = 0;

	forth->strings = malloc(sizeof(char*));
	forth->strings[0] = 0;

	return forth;
}

void forth_freeInstance(ForthInstance *forth) {
	for(int i = 0; forth->strings[i]; i++)
		free(forth->strings[i]);
	free(forth->strings);

	if(forth->addresses)
		free(forth->addresses);

	free(forth);
}

int forth_pop(ForthInstance *forth) {
	forth->sp--;
	if(forth->sp < 0) {
		printf("stack underflow !\n");
		forth->sp = 0;
		return 0;
	}
	else
		return forth->stack[forth->sp];
}

void forth_push(ForthInstance *forth, int n) {
	forth->sp++;
	if(forth->sp >= FORTH_STACK_SIZE) {
		printf("stack overflow !\n");
		forth->sp = FORTH_STACK_SIZE - 1;
	}
	else
		forth->stack[forth->sp-1] = n;
}

int forth_addString(ForthInstance *forth, char *s) {
	int l;
	for(l = 0; forth->strings[l]; l++);
	l++;

	forth->strings = realloc(forth->strings, sizeof(char*)*(l+1));
	forth->strings[l-1] = malloc(strlen(s)+1);
	strcpy(forth->strings[l-1], s);

	return l-1;
}
