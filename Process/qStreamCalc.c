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
}

void QSTREAM_CALC_addStat(QSTREAM_CALC* calc, void (*stat)(TIMEVALUE*, QUOTE*, void*), void* mem) {
	if (calc->len == calc->buffer) {
		calc->buffer += 10;
		void* oldStats = (void*)calc->stats;
		void* oldMemories = (void*)calc->memories;
		void* oldResults = (void*)calc->results;

		calc->stats = malloc(sizeof(void*) * calc->buffer);
		calc->memories = malloc(sizeof(void*) * calc->buffer);
		calc->results = malloc(sizeof(void*) * calc->buffer);

		memcpy((void*)calc->stats, oldStats, sizeof(void*) * calc->len);
		memcpy(calc->memories, oldMemories, sizeof(void*) * calc->len);
		memcpy(calc->results, oldResults, sizeof(void*) * calc->len);

		free(oldStats);
		free(oldMemories);
		free(oldResults);
	}

	calc->stats[calc->len] = stat;
	calc->memories[calc->len] = mem;
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
		free(calc->memories[i]);
	}
	free(calc->stats);
	free(calc->memories);
	free(calc->results);
}