#pragma warning(disable : 4996)

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "spymem.h"
#include "lists.h"

List* SP_heap;
List* SP_realls;
List* SP_frees;

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
		snprintf(acc, 4, "%02X ", ((unsigned char*) str)[i]);
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

static bool scanptr(const void * curptr, const void * tarptr) {
	return ((SP_Alloc*) curptr)->ptr == tarptr;
}

#ifdef DEBUG
	
	extern void* SP_malloc(size_t size, List* list) {
		void* ptr = malloc(size);
		SP_Alloc* alloc = (SP_Alloc*) calloc(1, sizeof(SP_Alloc));  /* LEAK NECESSARY FOR PRINT! */
		alloc->ptr = ptr;
		alloc->timestamp = clock();
		placelist(list->length, list, size, alloc);
		list->ids[list->length - 1] = 0;
		
		return ptr;
	}
	
	extern void* SP_realloc(const void* ptr, size_t size, List* list, List* real) {
		signed long idx = customscan(scanptr, ptr, list);
		SP_Reall* reall = (SP_Reall*) calloc(1, sizeof(SP_Reall));  /* LEAK NECESSARY FOR PRINT! */
		reall->from = ((SP_Alloc*) list->data[idx])->ptr;
		reall->frmlen = list->lengths[idx];
		void* pt = realloc(ptr, size);
		((SP_Alloc*) list->data[idx])->ptr = pt;
		reall->timestamp = ((SP_Alloc*) list->data[idx])->timestamp = clock();
		list->lengths[idx] = size;
		list->ids[idx] = 2;
		reall->to = ((SP_Alloc*) list->data[idx])->ptr;
		reall->tolen = list->lengths[idx];
		placelist(real->length, real, size, reall);
		/*placelist(idx, list, size, pt);*/
		
		return pt;
	}
	
	extern void* SP_calloc(size_t nitems, size_t size, List* list) {
		void* ptr = calloc(nitems, size);
		SP_Alloc* alloc = (SP_Alloc*) calloc(1, sizeof(SP_Alloc));  /* LEAK NECESSARY FOR PRINT! */
		alloc->ptr = ptr;
		alloc->timestamp = clock();
		placelist(list->length, list, nitems * size, alloc);
		list->ids[list->length - 1] = 1;

		return ptr;
	}
	
	extern void SP_free(const void* ptr, List* list) {
		signed long idx = customscan(scanptr, ptr, list);
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

extern char* SP_printheap(size_t xpose) {
	SP_Alloc tm;
	size_t cnt = intsize(SP_heap->length) + hexsize(SP_heap) * 2 + intsize(SIZE_MAX) * 3 + 15 + xpose,
		cnt2 = (intsize(SP_heap->length) + hexsize(SP_heap) * 2 + intsize(SIZE_MAX) * 2 + 14 + xpose) * SP_heap->length + 1;
	char* tmp = (char*) calloc(cnt, sizeof(char)),
		* ret = (char*) calloc(cnt2, sizeof(char));

	for (register size_t i = 0; i < SP_heap->length; i++) {
		tm = *((SP_Alloc*) SP_heap->data[i]);
		snprintf(tmp, cnt, "%zu,%ld: %p (%zu)[%lu] - %s\n", i, SP_heap->ids[i], tm.ptr, SP_heap->lengths[i], tm.timestamp, hexify(tm.ptr, SP_heap->lengths[i]));

		if (i) {
			strcat(ret, tmp);
		} else {
			strcpy(ret, tmp);
		}
	}

	free(tmp);

	return ret;
}

extern char* SP_printreall() {
	size_t cnt = intsize(SP_realls->length) + hexsize(SP_realls) * 2 + intsize(SIZE_MAX) * 2 + 17,
		cnt2 = (intsize(SP_realls->length) + hexsize(SP_realls) * 2 + intsize(SIZE_MAX) * 2 + 16) * SP_realls->length + 1;
	char* tmp = (char*) calloc(cnt, sizeof(char)),
		* ret = (char*) calloc(cnt2, sizeof(char));
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
	SP_frees = (List*) calloc(1, sizeof(List));
	initlist(SP_frees, 0);
}
