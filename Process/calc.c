#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "quote.h"

#include "calc.h"

void initCalc(struct calc* calc) {
	calc->len = 0;
	calc->buffer = 10;
	calc->stats = malloc(sizeof(void*) * calc->buffer);
	calc->memories = malloc(sizeof(void*) * calc->buffer);
	calc->results = malloc(sizeof(void*) * calc->buffer);
}

void addCalcStat(struct calc* calc, void (*stat)(struct TimePair* result, struct Quote* quote, void* memory), void* memory) {
	
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

void executeCalc(MYSQL_RES* mysql_res, int quotes, struct calc* calc) {
	struct Quote quote;
	int i, j;

	for (j = 0; j < calc->len; ++j) {
		calc->results[j] = malloc(sizeof(struct TimePair) * quotes);
	}

	for (i = 0; i < quotes; ++i) {
		if (!getQuote(mysql_res, &quote)) break;
		for (j = 0; j < calc->len; ++j) {
			(calc->stats[j])(calc->results[j] + i, &quote, calc->memories[j]);
		}
	}
}

void doCalc(char* series, char* query, struct calc* calc) {
	char* sql = getQuoteQuery(series, query);

	MYSQL_RES* results = requestQuotes(sql);
	free(sql);

	if (!results) {
		printf("Didn't get results: %s\n", sql);
		perror("doCalc");
	}

	executeCalc(results, mysql_num_rows(results), calc);
	mysql_free_result(results);
}

void freeCalc(struct calc* calc) {
	printf("free calc:: %i\n", calc->len);
	int i;
	for (i = 0; i < calc->len; ++i) {
		free(calc->results[i]);
		free(calc->memories[i]);
	}
	free(calc->stats);
	free(calc->memories);
	free(calc->results);
}