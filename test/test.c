#pragma warning(disable : 4996)

#include "spymem.h"
#include "lists.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif


#ifdef _WIN32
	#define sleep Sleep
#endif

#define debug(...) {\
	fprintf(stderr, __VA_ARGS__);\
	fflush(stderr);\
}


extern List* SP_heap;

int main() {
	SP_start();
	debug("%s\n", SP_printheap(50));
	char* f = (char*)smalloc(5);
	strcpy(f, "test");
	debug("\nf=smalloc(5).\n");
	debug("%s\n", SP_printheap(50));
	void* fr = smalloc(3);
	debug("fr=smalloc(3).\n");
	debug("%s\n", SP_printheap(50));
	sfree(fr);
	debug("sfree(fr).\n");
	debug("%s\n", SP_printheap(50));
	scalloc(2, 1);
	debug("scalloc(2, 1).\n");
	debug("%s\n", SP_printheap(50));
	f = srealloc(f, 6);
	debug("f=srealloc(f, 6).\n");
	debug("%s\n", SP_printheap(50));
	debug("%s\n", SP_printreall());
}
