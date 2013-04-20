#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "quote.h"

#include "calc.h"

void initCalc(struct Calc* calc) {
	calc->len = 0;
	calc->buffer = 10;
	calc->stats = malloc(sizeof(void*) * calc->buffer);
	calc->memories = malloc(sizeof(void*) * calc->buffer);
	calc->results = malloc(sizeof(void*) * calc->buffer);
}

void addCalcStat(struct Calc* calc, void (*stat)(struct TimePair* result, struct Quote* quote, void* memory), void* memory) {
	
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
	calc->memories[calc->len] = memory;
	++calc->len;
}

void executeCalc(struct DataConn* conn, int quotes, struct Calc* calc) {
	struct Quote quote;
	int i, j;

	int size = 0;

	for (j = 0; j < calc->len; ++j) {
		calc->results[j] = malloc(sizeof(struct TimePair) * quotes);
		size += sizeof(struct TimePair) * quotes;
	}

	printf("results size: %i\n", size);

	for (i = 0; i < quotes; ++i) {
		if (retreiveDataConnQuote(conn, &quote)) break;
		for (j = 0; j < calc->len; ++j) {
			(calc->stats[j])(calc->results[j] + i, &quote, calc->memories[j]);
		}
	}
}

void doCalc(struct DataConn* conn, char* series, char* query, struct Calc* calc) {
	char* sql = getQuoteQuery(series, query);
	int query_res = queryDataConn(conn, sql);
	if (query_res == -1) {
		printf("Didn't get results: %s\n", sql);
		printDataConnError(conn);
		exit(1);
	}

	free(sql);

	executeCalc(conn, query_res, calc);
	freeDataConnQuery(conn);
}

void freeCalc(struct Calc* calc) {
	int i;
	for (i = 0; i < calc->len; ++i) {
		free(calc->results[i]);
		free(calc->memories[i]);
	}
	free(calc->stats);
	free(calc->memories);
	free(calc->results);
}