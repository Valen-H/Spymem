#pragma once

#define TKN(t) #t

#ifndef DEBUG
	
	#define smalloc(size) malloc(size)
	#define sfree(ptr) free(ptr)
	#define sSP_Realloc(ptr, size) SP_Realloc(ptr, size)
	#define scalloc(nitems, size) calloc(nitems, size)
	#define SP_malloc(size, list) malloc(size)
	#define SP_free(ptr, list) free(ptr)
	#define SP_Realloc(ptr, size, list) SP_Realloc(ptrs, size)
	#define SP_calloc(nitems, size, list) calloc(nitems, size)
	
#else
	
	#include <stddef.h>
	#include <time.h>
	#include "lists.h"
	
	extern void * SP_malloc(size_t size, List * list);
	extern void * SP_realloc(const void * ptr, size_t size, List * list);
	extern void * SP_calloc(size_t nitems, size_t size, List * list);
	extern void SP_free(const void * ptr, List * list);
	extern void * smalloc(size_t size);
	extern void * sSP_Realloc(const void * ptr, size_t size);
	extern void * scalloc(size_t nitems, size_t size);
	extern void sfree(const void * ptr);
	
#endif

	extern List * SP_heap;
	extern List * SP_realls;
	extern List * SP_frees;

	extern void SP_start(void);
	
	typedef struct SP_Reall {
		const void * from,
			* to;
		size_t frmlen,
			tolen;
		clock_t timestamp;
	} SP_Reall;

	typedef struct SP_Alloc {
		const void * ptr;
		size_t size;
		clock_t timestamp;
	} SP_Alloc;

	typedef struct SP_Free {
		const SP_Alloc * make;
		clock_t timestamp;
	} SP_Free;

	extern char * SP_printreall(void);