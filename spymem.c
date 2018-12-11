#ifdef DEBUG
	
	#include <stdlib.h>
	#include <string.h>
	#include <stdio.h>
	#include "spymem.h"
	#include "lists.h"
	
	static List Main;
	static size_t cntr = 0;
	
	extern void* spmalloc(size_t size, List* const list) {
		void* ptr = malloc(size);
		signed char r = placelist(list->length, list, size, ptr);
		if (!r) list->ids[list->length] = cntr++;
		
		return ptr;
	}
	
	extern void* sprealloc(void* ptr, size_t size, List* const list) {
		signed long int idx = scanindex(ptr, list);
		void* pt = realloc(ptr, size);
		signed char r = placelist(idx, list, size, pt);
		if (!r) list->ids[idx] = cntr++;
		
		return pt;
	}
	
	extern void* spcalloc(size_t nitems, size_t size, List* const list) {
		void* ptr = calloc(nitems, size);
		signed char r = placelist(list->length, list, nitems * size, ptr);
		if (!r) list->ids[list->length] = cntr++;
		return ptr;
	}
	
	extern void spfree(void* ptr, List* const list) {
		signed long int idx = scanindex(ptr, list);
		rmlist(idx, list);
		free(ptr);
	}
	
	extern void* smalloc(size_t size) {
		return spmalloc(size, &Main);
	}
	extern void* srealloc(void* ptr, size_t size) {
		return sprealloc(ptr, size, &Main);
	}
	extern void* scalloc(size_t nitems, size_t size) {
		return spcalloc(nitems, size, &Main);
	}
	extern void sfree(void* ptr) {
		spfree(ptr, &Main);
	}
	
	extern const List * sgetmain() {
		Main = makelist(0);
		return &Main;
	}
	
#endif
