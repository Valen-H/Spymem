#pragma once

#define TKN(t) #t

#include <stddef.h>
#include <time.h>
#include "lists.h"

#ifndef DEBUG
	
	#define smalloc(size) malloc(size)
	#define sfree(ptr) free(ptr)
	#define srealloc(ptr, size) SP_realloc(ptr, size)
	#define scalloc(nitems, size) calloc(nitems, size)
	#define SP_malloc(size, list) malloc(size)
	#define SP_free(ptr, list) free(ptr)
	#define SP_realloc(ptr, size, list) SP_realloc(ptrs, size)
	#define SP_calloc(nitems, size, list) calloc(nitems, size)
	
#else
	
	extern void * SP_malloc(size_t size, List * list);
	extern void * SP_realloc(const void * ptr, size_t size, List* list, List* real);
	extern void * SP_calloc(size_t nitems, size_t size, List * list);
	extern void SP_free(const void * ptr, List * list, List * frees);
	extern void * smalloc(size_t size);
	extern void * srealloc(const void * ptr, size_t size);
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
			tolen,
			id,
			toid;
		clock_t frmtimestamp,
			totimestamp;
	} SP_Reall;

	typedef struct SP_Alloc {
		const void * ptr;
		clock_t timestamp;
		size_t id;
	} SP_Alloc;

	typedef struct SP_Free {
		const void * ptr;
		clock_t frmtimestamp,
			timestamp;
		size_t id;
	} SP_Free;

	extern char * SP_printreall(const List * reall);
	extern char * SP_printheap(const List * heap, signed long xpose);
	extern char * SP_printfree(const List * frees);
	extern char * sprintreall(void);
	extern char * sprintheap(signed long xpose);
	extern char * sprintfree(void);
