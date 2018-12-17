#pragma warning(disable : 4996)

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include "spymem.h"
#include "lists.h"

List* SP_heap;
List* SP_realls;
List* SP_frees;

static const size_t MAXPTRADDRSIZE = 30;
static size_t idcnt = 0;

static size_t hexsize(const void* const ptr) {
	char* ret = (char*) calloc(MAXPTRADDRSIZE, sizeof(char));
	register size_t i;
	
	snprintf(ret, MAXPTRADDRSIZE, "%p", ptr);
	i = strlen(ret);
	free(ret);
	
	return i; /* EXCLUDES \0 */
}

static char* hexify(const void* str, signed long delim) {
	size_t size = delim >= 0 ? delim : (strlen(str) + 1);
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
		alloc->id = idcnt++;
		placelist(list->length, list, size, alloc);
		list->ids[list->length - 1] = 0;
		
		return ptr;
	}
	
	extern void* SP_realloc(void* ptr, size_t size, List* list, List* real) {
		signed long idx = customscan(scanptr, ptr, list);
		void* pt;
		
		if (idx >= 0) {
			SP_Alloc* targ = ((SP_Alloc*) list->data[idx]);
			SP_Reall* reall = (SP_Reall*) calloc(1, sizeof(SP_Reall));  /* LEAK NECESSARY FOR PRINT! */
			reall->from = targ->ptr;
			reall->frmlen = list->lengths[idx];
			reall->frmtimestamp = targ->timestamp;
			pt = realloc(ptr, size);
			if (pt != NULL) {
				reall->to = targ->ptr = pt;
				reall->tolen = list->lengths[idx] = size;
				reall->totimestamp = targ->timestamp = clock();
			} else {
				reall->to = reall->from;
				reall->tolen = reall->frmlen;
				reall->totimestamp = reall->frmtimestamp;
			}
			reall->id = targ->id;
			targ->id = reall->toid = idcnt++;
			placelist(real->length, real, size, reall);
			real->ids[real->length - 1] = list->ids[idx];
			list->ids[idx] = 2;
		} else {
			pt = realloc(ptr, size);
		}
		
		return pt;
	}
	
	extern void* SP_calloc(size_t nitems, size_t size, List* list) {
		void* ptr = calloc(nitems, size);
		SP_Alloc* alloc = (SP_Alloc*) calloc(1, sizeof(SP_Alloc));  /* LEAK NECESSARY FOR PRINT! */
		alloc->ptr = ptr;
		alloc->timestamp = clock();
		alloc->id = idcnt++;
		placelist(list->length, list, nitems * size, alloc);
		list->ids[list->length - 1] = 1;
		
		return ptr;
	}
	
	extern void SP_free(void* ptr, List* list, List* frees) {
		signed long idx = customscan(scanptr, ptr, list);
		
		if (idx >= 0) {
			SP_Free* fr = (SP_Free*) calloc(1, sizeof(SP_Free));  /* LEAK NECESSARY FOR PRINT! */
			SP_Alloc* targ = ((SP_Alloc*) list->data[idx]);
			fr->ptr = targ->ptr;
			fr->frmtimestamp = targ->timestamp;
			fr->timestamp = clock();
			fr->id = targ->id;
			placelist(frees->length, frees, list->lengths[idx], fr);
			frees->ids[frees->length - 1] = list->ids[idx];
			rmlist(idx, list);
		}
		
		free(ptr);
	}
	
	extern void* smalloc(size_t size) {
		return SP_malloc(size, SP_heap);
	}

	extern void* srealloc(void* ptr, size_t size) {
		return SP_realloc(ptr, size, SP_heap, SP_realls);
	}

	extern void* scalloc(size_t nitems, size_t size) {
		return SP_calloc(nitems, size, SP_heap);
	}
	
	extern void sfree(void* ptr) {
		SP_free(ptr, SP_heap, SP_frees);
	}

#endif

extern char* SP_printheap(const List* heap, signed long xpose) {
	SP_Alloc tm;
	size_t cnt = intsize(heap->length) + hexsize(heap) + intsize(SIZE_MAX) * 4 + 20 + xpose,
		cnt2 = (intsize(heap->length) + hexsize(heap) + intsize(SIZE_MAX) * 4 + 19 + xpose) * heap->length + 1;
	char* tmp = (char*) calloc(cnt, sizeof(char)),
		* ret = (char*) calloc(cnt2, sizeof(char));
	
	for (register size_t i = 0; i < heap->length; i++) {
		tm = *((SP_Alloc*) heap->data[i]);
		snprintf(tmp, cnt - 1, "%zu,%ld,%zu: %p (%zu)[%lu] - %s", i, heap->ids[i], tm.id, tm.ptr, heap->lengths[i], tm.timestamp, hexify(tm.ptr, heap->lengths[i]));
		strcat(tmp, "\n");
		
		if (i) {
			strcat(ret, tmp);
		} else {
			strcpy(ret, tmp);
		}
	}
	
	free(tmp);
	
	return ret;
}

extern char* SP_printreall(const List* realls) {
	SP_Reall tm;
	size_t cnt = intsize(realls->length) + hexsize(realls) * 2 + intsize(SIZE_MAX) * 7 + 31,
		cnt2 = (intsize(realls->length) + hexsize(realls) * 2 + intsize(SIZE_MAX) * 7 + 30) * realls->length + 1;
	char* tmp = (char*) calloc(cnt, sizeof(char)),
		* ret = (char*) calloc(cnt2, sizeof(char));
	
	for (register size_t i = 0; i < realls->length; i++) {
		tm = *((SP_Reall*) realls->data[i]);
		snprintf(tmp, cnt, "%zu,%ld,%lu->%lu: %p (%zu)[%lu] -> %p (%zu)[%lu]\n", i, realls->ids[i], tm.id, tm.toid, tm.from, tm.frmlen, tm.frmtimestamp, tm.to, tm.tolen, tm.totimestamp);
		
		if (i) {
			strcat(ret, tmp);
		} else {
			strcpy(ret, tmp);
		}
	}
	
	free(tmp);
	
	return ret;
}

extern char* SP_printfree(const List* frees) {
	SP_Free tm;
	size_t cnt = intsize(frees->length) + hexsize(frees) + intsize(SIZE_MAX) * 4 + 20,
		cnt2 = (intsize(frees->length) + hexsize(frees) + intsize(SIZE_MAX) * 4 + 19) * frees->length + 1;
	char* tmp = (char*) calloc(cnt, sizeof(char)),
		*ret = (char*) calloc(cnt2, sizeof(char));
	
	for (register size_t i = 0; i < frees->length; i++) {
		tm = *((SP_Free*) frees->data[i]);
		snprintf(tmp, cnt, "%zu,%ld,%zu: %p (%zu)[%lu][%lu]\n", i, frees->ids[i], tm.id, tm.ptr, frees->lengths[i], tm.frmtimestamp, tm.timestamp);
		
		if (i) {
			strcat(ret, tmp);
		} else {
			strcpy(ret, tmp);
		}
	}
	
	free(tmp);
	
	return ret;
}

extern char* sprintheap(signed long xpose) {
	return SP_printheap(SP_heap, xpose);
}

extern char* sprintreall() {
	return SP_printreall(SP_realls);
}

extern char* sprintfree() {
	return SP_printfree(SP_frees);
}

extern void SP_start() {
	SP_heap = (List*) calloc(1, sizeof(List));
	initlist(SP_heap, 0);
	SP_realls = (List*) calloc(1, sizeof(List));
	initlist(SP_realls, 0);
	SP_frees = (List*) calloc(1, sizeof(List));
	initlist(SP_frees, 0);
}
