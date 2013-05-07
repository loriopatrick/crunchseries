#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "network.h"
#include "database.h"
#include "quote.h"
#include "statGraph.h"
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

	Todo: How should settings be passed through?

	Data format
	- number of steps
	- {foreach step}
		- number of stats
		- {foreach stat}
			- stat number
			- number of settings
			- {foreach setting}
				- byte size
				- value
			- number of inputs for stat
			- {foreach input}
				- input map corresponding to last output

*/

void testStatGraph() {
	char data[300];
	int pos = 0;

	int temp = 2;
	memcpy(data, &temp, sizeof(int)); // number of steps (2)
	pos += sizeof(int);

	temp = 1;
	memcpy(data+pos, &temp, sizeof(int)); // first step: number of stats (1)
	pos += sizeof(int);

	memcpy(data+pos, &temp, sizeof(int)); // stat id (1[quote query])
	pos += sizeof(int);

	memcpy(data+pos, &temp, sizeof(int)); // number of settings (1)
	pos += sizeof(int);

	char* querya = getQuoteQSRange("GOOG", 0, 99999999);
	char* query = getQuoteQS("eod", querya);
	free(querya);

	temp = strlen(query);
	memcpy(data+pos, &temp, sizeof(int)); // byte size for setting
	pos += sizeof(int);

	memcpy(data+pos, query, temp); // setting
	pos += temp;

	free(query);

	temp = 0;
	memcpy(data+pos, &temp, sizeof(int)); // number of inputs (0)
	pos += sizeof(int);

	// -------- new step --------

	temp = 1;
	memcpy(data+pos, &temp, sizeof(int)); // number of stats id (2[moving average])
	pos += sizeof(int);

	temp = 2;
	memcpy(data+pos, &temp, sizeof(int)); // stat id (2[moving average])
	pos += sizeof(int);

	temp = 1;
	memcpy(data+pos, &temp, sizeof(int)); // number of settings
	pos += sizeof(int);

	temp = sizeof(int);
	memcpy(data+pos, &temp, sizeof(int)); // byte size of setting
	pos += sizeof(int);

	temp = 20;
	memcpy(data+pos, &temp, sizeof(int)); // setting
	pos += sizeof(int);

	temp = 1;
	memcpy(data+pos, &temp, sizeof(int)); // number of inputs (1)
	pos += sizeof(int);

	temp = 0;
	memcpy(data+pos, &temp, sizeof(int)); // input map (0)
	pos += sizeof(int);


	struct graph* graph = buildGraph(0, data, pos);

	printf("step: %i\n", getGraphStep(graph));

	return;
}

int main(int args, char** argv) {
	// DB_connect();

	testStatGraph();

	// DB_close();
	// return 0;
}