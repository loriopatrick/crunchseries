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

int main(int args, char** argv) {
	DB_connect();

	testStats();

	DB_close();
	return 0;
}