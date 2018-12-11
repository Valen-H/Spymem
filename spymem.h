#pragma once

#define TKN(t) #t

#ifndef DEBUG
	
	#define smalloc(size) malloc(size)
	#define sfree(ptr) free(ptr)
	#define srealloc(ptr, size) realloc(ptr, size)
	#define scalloc(nitems, size) calloc(nitems, size)
	#define spmalloc(size, list) malloc(size)
	#define spfree(ptr, list) free(ptr);
	#define sprealloc(ptr, size, list) realloc(ptrs, size)
	#define spcalloc(nitems, size, list) calloc(nitems, size)
	
#else
	
	#include <stddef.h>
	#include "lists.h"
	
	extern void * spmalloc(size_t size, List * const list);
	extern void * sprealloc(void * ptr, size_t size, List * const list);
	extern void * spcalloc(size_t nitems, size_t size, List * const list);
	extern void spfree(void * ptr, List * const list);
	extern void * smalloc(size_t size);
	extern void * srealloc(void * ptr, size_t size);
	extern void * scalloc(size_t nitems, size_t size);
	extern void sfree(void * ptr);
	extern const List * sgetmain(void);
	
#endif
