#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "network.h"
#include "database.h"
#include "quote.h"
#include "stats.h"

void testQuoteQuery() {
	char* query = getQuoteQS("eod", "WHERE symbol='GOOG'");
	printf("QUERY:: %s\n", query);

	QUOTES* quotes = getQuotes(DB_query(query));
	printf("Count: %i\n", quotes->count);

	int i;
	for (i = 0; i < quotes->count; ++i) {
		printQuote(quotes, i);
	}
}

void printArray(double* array, int len) {
	int i;
	for (i = 0; i < len; ++i) {
		printf("%f\n", array[i]);
	}
}

void testStats() {
	char* query = getQuoteQS("eod", "WHERE symbol='GOOG'");
	printf("QUERY:: %s\n", query);

	QUOTES* quotes = getQuotes(DB_query(query));
	printf("Count: %i\n", quotes->count);

	double* res = exponentialMovingAverage(quotes->close, quotes->count, 12);
	printArray(res, quotes->count);
}

/*
	Data format:
	- number of steps
	- {foreach step}
		- number of stats
		- {foreach stat}
			- stat number
			- number of inputs for stat
			- input map for stat

*/

void testStatGraph() {
	char data[100];
	int pos = 0;

	int temp = 2;
	memcpy(data, &temp, sizeof(int)); // number of steps (2)
	pos += sizeof(int);

	temp = 1;
	memcpy(data+pos, &temp, sizeof(int)); // first step: number of stats (1)
	pos += sizeof(int);

	memcpy(data+pos, &temp, sizeof(int)); // stat id (1[quote query])
	pos += sizeof(int);

	memcpy(data+pos, &temp, sizeof(int)); // number of inputs (1)
	pos += sizeof(int);

	temp = 0;
	memcpy(data+pos, &temp, sizeof(int)); // input map (0)
	pos += sizeof(int);




	memcpy(data+pos, &temp, sizeof(int)); // stat id (2[moving average])
	pos += sizeof(int);

	memcpy(data+pos, &temp, sizeof(int)); // number of inputs (1)
	pos += sizeof(int);

	temp = 0;
	memcpy(data+pos, &temp, sizeof(int)); // input map (0)
	pos += sizeof(int);
}

int main(int args, char** argv) {
	DB_connect();

	testStats();

	DB_close();
	return 0;
}