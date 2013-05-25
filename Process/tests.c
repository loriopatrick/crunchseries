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

	char* querya = getQuoteQSRange("GOOG", 0, 9999999999);
	char* query = getQuoteQS("eod", querya);
	free(querya);

	temp = strlen(query) + 1;
	memcpy(data+pos, &temp, sizeof(int)); // byte size for setting
	pos += sizeof(int);

	memcpy(data+pos, query, temp); // setting
	pos += temp;

	free(query);

	temp = 0;
	memcpy(data+pos, &temp, sizeof(int)); // number of inputs (0)
	pos += sizeof(int);

	temp = 6;
	memcpy(data+pos, &temp, sizeof(int)); // number of outputs
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
	memcpy(data+pos, &temp, sizeof(int)); // input map which stat?
	pos += sizeof(int);

	temp = 4;
	memcpy(data+pos, &temp, sizeof(int)); // input map which output?
	pos += sizeof(int);

	temp = 1;
	memcpy(data+pos, &temp, sizeof(int)); // number of outputs
	pos += sizeof(int);


	// StatGraph* graph = buildGraph(data);

	char data2[] = {3, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 101, 111, 100, 4, 0, 0, 0, 71, 79, 79, 71, 8, 0, 0, 0, 0, 224, 255, 161, 148, 26, 109, 66, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 24, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 24, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255};

	StatGraph* testGraph = buildGraph(data2);

	// printf("step: %i\n", getGraphStep(graph));

	// executeGraphStep(graph, 0);
	// executeGraphStep(graph, 1);

	// printArray(graph->steps[1].stats[0].outputs[0].values, graph->steps[1].stats[0].outputs[0].len);
	return;
}

int main(int args, char** argv) {
	DB_connect();

	// testQuoteQuery();
	testStatGraph();

	DB_close();
}