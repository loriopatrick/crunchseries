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

	char data[] = {4,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,4,0,0,0,1,0,0,0,4,0,0,0,24,0,0,0,1,0,0,0,2,0,0,0,4,0,0,0,1,0,0,0,4,0,0,0,3,0,0,0,101,111,100,4,0,0,0,71,79,79,71,8,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,224,255,161,148,26,109,66,0,0,0,0,2,0,0,0,1,0,0,0,4,0,0,0,24,0,0,0,1,0,0,0,2,0,0,0,4,0,0,0,0,0,0,0};

	StatGraph* graph = buildGraph(data);

	// printf("\n\n\n");
	printf("\n\n\n");
	// printf("\n\n\n");

	runGraph(graph);

	// printf("Integ:: %i == 3? \n", graph[0].steps[1].stats[0].stat);
	// executeGraphStep(graph, 0);
	// printf("Integ:: %i == 3? \n", graph[0].steps[1].stats[0].stat);
	// executeGraphStep(graph, 1);
	// printf("Integ:: %i == 3? \n", graph[0].steps[1].stats[0].stat);
	// executeGraphStep(graph, 2);

	int stat = graph->head, output = 2;
	printf("%i\n", stat);
	// printf("Array len: %i\n", graph[0].steps[step].stats[stat].outputs[output].len);
	printArray(graph->stats[stat].outputs[output].values, graph->stats[stat].outputs[output].len);

}

int main(int args, char** argv) {
	DB_connect();

	// testQuoteQuery();
	testStatGraph();

	DB_close();
}