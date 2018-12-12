#pragma once

#define TKN(t) #t

#ifndef DEBUG
	
	#define smalloc(size) malloc(size)
	#define sfree(ptr) free(ptr)
	#define srealloc(ptr, size) realloc(ptr, size)
	#define scalloc(nitems, size) calloc(nitems, size)
	#define SP_malloc(size, list) malloc(size)
	#define SP_free(ptr, list) free(ptr);
	#define SP_realloc(ptr, size, list) realloc(ptrs, size)
	#define SP_calloc(nitems, size, list) calloc(nitems, size)
	
#else
	
	#include <stddef.h>
	#include "lists.h"
	
	extern List* SP_heap;
	
	extern void * SP_malloc(size_t size, List * list);
	extern void * SP_realloc(void * ptr, size_t size, List * list);
	extern void * SP_calloc(size_t nitems, size_t size, List * list);
	extern void SP_free(void * ptr, List * list);
	extern void * smalloc(size_t size);
	extern void * srealloc(void * ptr, size_t size);
	extern void * scalloc(size_t nitems, size_t size);
	extern void sfree(void * ptr);
	extern void SP_start(void);
	
#endif
