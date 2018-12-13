#pragma warning(disable : 4996)

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "spymem.h"
#include "lists.h"

static const size_t MAXPTRADDRSIZE = 30;

static size_t hexsize(const void* const ptr) {
	char* ret = (char*) calloc(MAXPTRADDRSIZE, sizeof(char));
	register size_t i;

	snprintf(ret, MAXPTRADDRSIZE, "%p", ptr);
	i = strlen(ret);
	free(ret);

	return i; /* EXCLUDES \0 */
}

static char* hexify(void* str, size_t delim) {
	size_t size = delim ? delim : (strlen(str) + 1);
	char* ret = (char*) calloc(size * 4 + 1, sizeof(char)),
		*acc = (char*) calloc(4, sizeof(char));

	memset(ret, 0, size * 3 + 1);

	for (register size_t i = 0; i < size; i++) {
		snprintf(acc, 4, "%02X ", ((char*) str)[i]);
		if (i) {
			strcat(ret, acc);
		} else {
			strcpy(ret, acc);
		}
	}

	free(acc);

	return ret;
}

static size_t intsize(register long value) {
	register size_t l = !value;

	while (value >= 1) {
		l++;
		value /= 10;
	}

	return l;
}

#ifdef DEBUG
	
	List* SP_heap;
	List* SP_realls;
	
	extern void* SP_malloc(size_t size, List* list) {
		void* ptr = malloc(size);
		placelist(list->length, list, size, ptr);
		list->ids[list->length - 1] = 0;
		
		return ptr;
	}
	
	extern void* SP_realloc(const void* ptr, size_t size, List* list, List* real) {
		signed long idx = scanindex(ptr, list);
		SP_Reall* reall = (SP_Reall*) calloc(1, sizeof(SP_Reall));  /* LEAK NECESSARY FOR PRINT! */
		reall->from = ptr;
		reall->frmlen = list->lengths[idx];
		void* pt = realloc(ptr, size);
		reall->to = pt;
		reall->tolen = size;
		placelist(real->length, real, size, reall);
		placelist(idx, list, size, pt);
		list->ids[idx] = 2;
		
		return pt;
	}
	
	extern void* SP_calloc(size_t nitems, size_t size, List* list) {
		void* ptr = calloc(nitems, size);
		placelist(list->length, list, nitems * size, ptr);
		list->ids[list->length - 1] = 1;

		return ptr;
	}
	
	extern void SP_free(const void* ptr, List* list) {
		signed long idx = scanindex(ptr, list);
		rmlist(idx, list);
		free(ptr);
	}
	
	extern void* smalloc(size_t size) {
		return SP_malloc(size, SP_heap);
	}

	extern void* srealloc(const void* ptr, size_t size) {
		return SP_realloc(ptr, size, SP_heap, SP_realls);
	}

	extern void* scalloc(size_t nitems, size_t size) {
		return SP_calloc(nitems, size, SP_heap);
	}

	extern void sfree(const void* ptr) {
		SP_free(ptr, SP_heap);
	}

#endif

extern char* SP_printreall() {
	size_t cnt = intsize(SP_realls->length) + hexsize(SP_realls) * 2 + intsize(SIZE_MAX) * 2 + 17,
		cnt2 = (intsize(SP_realls->length) + hexsize(SP_realls) * 2 + intsize(SIZE_MAX) * 2 + 16) * SP_realls->length + 1;
	char* tmp = (char*) calloc(cnt, sizeof(char)),
		*ret = (char*) calloc(cnt2, sizeof(char));
	SP_Reall tm;
	
	for (register size_t i = 0; i < SP_realls->length; i++) {
		tm = *((SP_Reall*) SP_realls->data[i]);
		snprintf(tmp, cnt, "%zu, %p (%zu) -> %p (%zu)\n", i, tm.from, tm.frmlen, tm.to, tm.tolen);
		
		if (i) {
			strcat(ret, tmp);
		} else {
			strcpy(ret, tmp);
		}
	}
	
	free(tmp);
	
	return ret;
}

extern void SP_start() {
	SP_heap = (List*) calloc(1, sizeof(List));
	initlist(SP_heap, 0);
	SP_realls = (List*) calloc(1, sizeof(List));
	initlist(SP_realls, 0);
}

/**
 * time.h  ->  clock() / CLOCKS_PER_SEC  ,  for timestamps
 */
