#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quote.h"
#include "database.h"

#include "qStreamCalc.h"

void QSTREAM_CALC_init(QSTREAM_CALC* calc) {
	calc->len = 0;
	calc->buffer = 10;
	calc->stats = malloc(sizeof(void*) * calc->buffer);
	calc->memories = malloc(sizeof(void*) * calc->buffer);
	calc->results = malloc(sizeof(void*) * calc->buffer);
	calc->free_mems = malloc(sizeof(void*) * calc->buffer);
}

void QSTREAM_CALC_addStat(QSTREAM_CALC* calc, void (*stat)(TIMEVALUE*, QUOTE*, void*), void* mem, void(*free_mem)(void*)) {
	if (calc->len == calc->buffer) {
		calc->buffer += 10;
		void* old_stats = (void*)calc->stats;
		void* old_memories = (void*)calc->memories;
		void* old_results = (void*)calc->results;
		void* old_frees = (void*)calc->free_mems;

		calc->stats = malloc(sizeof(void*) * calc->buffer);
		calc->memories = malloc(sizeof(void*) * calc->buffer);
		calc->results = malloc(sizeof(void*) * calc->buffer);
		calc->free_mems = malloc(sizeof(void*) * calc->buffer);

		memcpy((void*)calc->stats, old_stats, sizeof(void*) * calc->len);
		memcpy(calc->memories, old_memories, sizeof(void*) * calc->len);
		memcpy(calc->results, old_results, sizeof(void*) * calc->len);
		memcpy((void*)calc->free_mems, old_results, sizeof(void*) * calc->len);

		free(old_stats);
		free(old_memories);
		free(old_results);
		free(old_frees);
	}

	calc->stats[calc->len] = stat;
	calc->memories[calc->len] = mem;
	calc->free_mems[calc->len] = free_mem;
	++calc->len;
}

void QSTREAM_CALC_execute(QSTREAM_CALC* calc, DB_RES* res, int quotes) {
	QUOTE quote;
	int i, j;

	for (j = 0; j < calc->len; ++j) {
		calc->results[j] = malloc(sizeof(TIMEVALUE) * quotes);
	}

	for (i = 0; i < quotes; ++i) {
		if (!QUOTE_get(res, &quote)) break;
		for (j = 0; j < calc->len; ++j) {
			(calc->stats[j])(calc->results[j] + i, &quote, calc->memories[j]);
		}
	}
}

int QSTREAM_CALC_do(QSTREAM_CALC* calc, char* series, char* query) {
	char* sql = QUOTE_getQS(series, query);
	DB_RES* res = DB_query(sql);
	if (!res) {
		printf("Didn't get results: %s\n", sql);
		exit(1);
	}

	free(sql);

	int items = DB_numRows(res);
	QSTREAM_CALC_execute(calc, res, items);
	DB_freeRes(res);

	return items;
}

void QSTREAM_CALC_free(QSTREAM_CALC* calc) {
	int i;
	for (i = 0; i < calc->len; ++i) {
		free(calc->results[i]);
		if (calc->free_mems[i]) (calc->free_mems[i])(calc->memories[i]);
		free(calc->memories[i]);
	}
	free(calc->stats);
	free(calc->memories);
	free(calc->results);
	free(calc->free_mems);
}