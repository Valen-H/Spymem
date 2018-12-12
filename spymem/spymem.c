#ifdef DEBUG
	
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include <stdio.h>
	#include "spymem.h"
	#include "lists.h"
	
	List* SP_heap;
	static size_t cntr = 0;
	
	extern void* SP_malloc(size_t size, List* list) {
		void* ptr = malloc(size);
		bool r = placelist(list->length, list, size, ptr);
		if (!r) list->ids[list->length] = cntr++;
		
		return ptr;
	}
	
	extern void* SP_realloc(void* ptr, size_t size, List* list) {
		signed long idx = scanindex(ptr, list);
		void* pt = realloc(ptr, size);
		bool r = placelist(idx, list, size, pt);
		if (!r) list->ids[idx] = cntr++;
		
		return pt;
	}
	
	extern void* SP_calloc(size_t nitems, size_t size, List* list) {
		void* ptr = calloc(nitems, size);
		bool r = placelist(list->length, list, nitems * size, ptr);
		if (!r) list->ids[list->length] = cntr++;
		return ptr;
	}
	
	extern void SP_free(void* ptr, List* list) {
		signed long idx = scanindex(ptr, list);
		rmlist(idx, list);
		free(ptr);
	}
	
	extern void* smalloc(size_t size) {
		return SP_malloc(size, SP_heap);
	}

	extern void* srealloc(void* ptr, size_t size) {
		return SP_realloc(ptr, size, SP_heap);
	}

	extern void* scalloc(size_t nitems, size_t size) {
		return SP_calloc(nitems, size, SP_heap);
	}

	extern void sfree(void* ptr) {
		SP_free(ptr, SP_heap);
	}

	extern void SP_start() {
		SP_heap = (List*) calloc(1, sizeof(List));
		initlist(SP_heap, 0);
	}

#endif
