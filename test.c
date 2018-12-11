#include "spymem.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
	const List * list = sgetmain();
	
	void * f = smalloc(5);
	printf("\nf=smalloc(5).\nlength: %lu\n", list->length);
	
	printf("%s\n", printlist(list));
	
	void * fr = smalloc(3);
	printf("fr=smalloc(3).\nlength: %lu\n", list->length);
	
	printf("%s\n", printlist(list));
	
	sfree(fr);
	
	printf("sfree(fr).\nlength: %lu\n", list->length);
	
	scalloc(2, 1);
	
	f = srealloc(f, 6);
	
	printf("%s\n", printlist(list));
}
