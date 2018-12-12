#pragma warning(disable : 4996)

#include "spymem.h"
#include "lists.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define debug(...) {\
	fprintf(stderr, __VA_ARGS__);\
	fflush(stderr);\
}

extern List* SP_heap;

int main() {
	SP_start();
	char* f = (char*)smalloc(5);
	strcpy(f, "test");
	debug("\nf=smalloc(5).\n");
	debug("%s\n", printlist(SP_heap, 50));
	void* fr = smalloc(3);
	debug("fr=smalloc(3).\n");
	debug("%s\n", printlist(SP_heap, 50));
	sfree(fr);
	debug("sfree(fr).\n");
	debug("%s\n", printlist(SP_heap, 50));
	scalloc(2, 1);
	debug("scalloc(2, 1).\n");
	debug("%s\n", printlist(SP_heap, 50));
	f = srealloc(f, 6);
	debug("srealloc(f, 6).\n");
	debug("%s\n", printlist(SP_heap, 50));
}
