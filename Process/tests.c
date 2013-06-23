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

void testStatGraph() {

	char data[] = {2,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,4,0,0,0,3,0,0,0,101,111,100,4,0,0,0,71,79,79,71,8,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,224,255,161,148,26,109,66,0,0,0,0,0,0,0,0};

	StatGraph* graph = buildGraph(data);
	printf("\n\n\n");

	runGraph(graph);
	
	int stat = graph->head, output = 0;
	printf("%i\n", stat);
	printArray(graph->stats[stat].outputs[output].values, graph->stats[stat].outputs[output].len);

}

int main(int args, char** argv) {
	DB_connect();
	testStatGraph();
	DB_close();
}