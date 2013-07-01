#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "quote.h"
#include "stats.h"
#include "arithmetic_stats.h"

#include "statGraph.h"

#define DEBUG

/*

	Stat data schema
	================

	Format:
		int # of nodes
		{foreach node}
			int statId
			int # of settings
			{foreach setting}
				int # setting size
				char[] setting value
			int # of inputs
			{foreach input}
				int stat number in array
				int output
		int head
*/

int min(int a, int b) {
	if (a < b) return a;
	return b;
}

int buildStat(struct _statGraph_stat* stat, void* data) {
	int pos = 0;

	memcpy(&stat->stat, data, sizeof(int)); pos += sizeof(int);
	#ifdef DEBUG
		printf("\tstatId: %i\n", stat->stat);
	#endif

	memcpy(&stat->num_settings, data + pos, sizeof(int)); pos += sizeof(int);
	#ifdef DEBUG
		printf("\tnumber of settings: %i\n", stat->num_settings);
	#endif

	stat->settings = malloc(sizeof(struct _statGraph_setting) * stat->num_settings);
	int i;
	for (i = 0; i < stat->num_settings; ++i) {
		memcpy(&stat->settings[i].len, data + pos, sizeof(int)); pos += sizeof(int);
		stat->settings[i].data = malloc(stat->settings[i].len);
		memcpy(stat->settings[i].data, data + pos, stat->settings[i].len); pos += stat->settings[i].len;
	}

	memcpy(&stat->num_inputs, data + pos, sizeof(int)); pos += sizeof(int);
	#ifdef DEBUG
		printf("\tnumber of inputs: %i\n", stat->num_inputs);
	#endif

	stat->inputs = malloc(sizeof(struct _statGraph_input) * stat->num_inputs);
	memcpy(stat->inputs, data + pos, sizeof(struct _statGraph_input) * stat->num_inputs); pos += sizeof(struct _statGraph_input) * stat->num_inputs;

	stat->outputs = 0;
	stat->num_outputs = 0;
	return pos;
}

StatGraph* buildGraph(void* data) {
	#ifdef DEBUG
		printf("----- start:::buildGraph\n");
	#endif

	StatGraph* res = malloc(sizeof(StatGraph));

	int pos = 0;
	memcpy(&res->num_stats, data+pos, sizeof(int)); pos += sizeof(int);

	#ifdef DEBUG
		printf("# of stats: %i\n", res->num_stats);
	#endif

	res->stats = malloc(sizeof(struct _statGraph_stat) * res->num_stats);

	int i;
	for (i = 0; i < res->num_stats; ++i) {
		pos += buildStat(res->stats + i, data + pos);
		#ifdef DEBUG
			printf("\n");
		#endif
	}

	memcpy(&res->head, data+pos, sizeof(int)); pos += sizeof(int);

	#ifdef DEBUG
		printf("head id: %i\n", res->head);
	#endif

	#ifdef DEBUG
		printf("----- end:::buildGraph\n");
	#endif

	return res;
}

struct _statGraph_output* getInput(int stat, int output, StatGraph* graph) {
	runStat(graph->stats + stat, graph);
	return graph->stats[stat].outputs + output;
}

// void runSingleStat(struct _statGraph_stat* stat, StatGraph* graph, double* (*stat)(double*, int, int)) {

// }

int runStat(struct _statGraph_stat* stat, StatGraph* graph) {
	if (stat->outputs) return 0;

	if (stat->stat == 0) {
		#ifdef DEBUG
			printf("\tstat: 0: Copy Multiple Outputs\n");
		#endif

		int num_outputs = stat->num_inputs;
		stat->outputs = malloc(sizeof(struct _statGraph_output) * num_outputs);

		int i;
		for (i = 0; i < num_outputs; ++i) {
			struct _statGraph_output* series = getInput(stat->inputs[i].stat,
																	stat->inputs[i].output,
																	graph);

			printf("stat: %i, output: %i -> %i\n", stat->inputs[i].stat, stat->inputs[i].output, i);

			stat->outputs[i].len = series->len;
			stat->outputs[i].values = series->values;
		}

		return 0;
	}

	if (stat->stat == 1) {
		#ifdef DEBUG
			printf("\tstat: 1: SQL Query\n");
		#endif

		char* series = stat->settings[0].data;
		char* symbol = stat->settings[1].data;
		double begin = ((double*)stat->settings[2].data)[0];
		double end = ((double*)stat->settings[3].data)[0];

		char* endQ = getQuoteQSRange(symbol, begin, end);
		char* query = getQuoteQS(series, endQ);

		#ifdef DEBUG
			printf("\tquery: %s\n", query);
		#endif


		QUOTES* quotes = getQuotesByQuery(query);
		#ifdef DEBUG
			printf("\tquery: %s\n\t# of results: %i\n", query, quotes->count);
		#endif

		stat->outputs = malloc(sizeof(struct _statGraph_output) * 6);
		stat->outputs[0].values = quotes->utime; stat->outputs[0].len = quotes->count;
		stat->outputs[1].values = quotes->high; stat->outputs[1].len = quotes->count;
		stat->outputs[2].values = quotes->low; stat->outputs[2].len = quotes->count;
		stat->outputs[3].values = quotes->open; stat->outputs[3].len = quotes->count;
		stat->outputs[4].values = quotes->close; stat->outputs[4].len = quotes->count;
		stat->outputs[5].values = quotes->volume; stat->outputs[5].len = quotes->count;
		stat->num_outputs = 6;

		free(endQ);
		free(query);

		return 0;
	}

	if (stat->stat == 2) {
		#ifdef DEBUG
			printf("\tstat: 2: Simple Moving Average\n");
		#endif

		int* period_size = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		double* res = simpleMovingAverage(series->values, series->len, *period_size);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 3) {
		#ifdef DEBUG
			printf("\tstat: 3: Exponential Moving Average\n");
		#endif

		int* period_size = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		double* res = exponentialMovingAverage(series->values, series->len, *period_size);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].values = res;
		stat->outputs[0].len = series->len;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 4) {
		#ifdef DEBUG
			printf("\tstat: 4: Standard Deviation\n");
		#endif

		int* period_size = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		double* res = standardDeviation(series->values, series->len, *period_size);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 5) {
		#ifdef DEBUG
			printf("\tstat: 5: Slope\n");
		#endif

		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		double* res = slope(series->values, series->len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 6) {
		#ifdef DEBUG
			printf("\tstat: 6: Percent Difference\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);

		double* res = percentDifference(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output) * 2);
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 7) {
		#ifdef DEBUG
			printf("\tstat: 7: Trend Line\n");
		#endif

		int* period_size = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);
		
		double* res = trendLine(series->values, series->len, *period_size);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 8) {
		#ifdef DEBUG
			printf("\ta stat: 8: Add Number\n");
		#endif

		int* number = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);
		
		double* res = addNumber(series->values, series->len, *number);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 9) {
		#ifdef DEBUG
			printf("\ta stat: 9: Multiply Number\n");
		#endif

		int* number = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);
		
		double* res = multiplyNumber(series->values, series->len, *number);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 10) {
		#ifdef DEBUG
			printf("\ta stat: 10: Power Number\n");
		#endif

		int* number = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);
		
		double* res = powerNumber(series->values, series->len, *number);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 11) {
		#ifdef DEBUG
			printf("\ta stat: 11: Power Number\n");
		#endif

		int* number = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);
		
		double* res = powerNumber(series->values, series->len, *number);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 12) {
		#ifdef DEBUG
			printf("\ta stat: 12: Exponentiate Number\n");
		#endif

		int* number = stat->settings[0].data;
		struct _statGraph_output* series = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);
		
		double* res = exponentiateNumber(series->values, series->len, *number);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = series->len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 13) { // todo:: fix
		#ifdef DEBUG
			printf("\ta stat: 13: Sum\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);
		
		double* res = sum(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 14) {
		#ifdef DEBUG
			printf("\ta stat: 14: Sum\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);
		
		double* res = sum(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 15) {
		#ifdef DEBUG
			printf("\ta stat: 15: Difference\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);
		
		double* res = difference(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 16) {
		#ifdef DEBUG
			printf("\ta stat: 16: Product\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);
		
		double* res = product(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 17) {
		#ifdef DEBUG
			printf("\ta stat: 17: Quotient\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);
		
		double* res = quotient(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 18) {
		#ifdef DEBUG
			printf("\ta stat: 18: Quotient\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);
		
		double* res = quotient(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	if (stat->stat == 19) {
		#ifdef DEBUG
			printf("\ta stat: 19: Power\n");
		#endif

		struct _statGraph_output* series_a = getInput(stat->inputs[0].stat,
																stat->inputs[0].output,
																graph);

		struct _statGraph_output* series_b = getInput(stat->inputs[1].stat,
																stat->inputs[1].output,
																graph);

		int len = min(series_a->len, series_b->len);
		
		double* res = power(series_a->values, series_b->values, len);
		stat->outputs = malloc(sizeof(struct _statGraph_output));
		stat->outputs[0].len = len;
		stat->outputs[0].values = res;
		stat->num_outputs = 1;

		return 0;
	}

	return 1;
}

int runGraph(StatGraph* graph) {
	return runStat(graph->stats + graph->head, graph);
}

void freeStat(struct _statGraph_stat* stat) {
	int i;

	for (i = 0; i < stat->num_settings; ++i) { free(stat->settings[i].data); }
	free(stat->settings);
	for (i = 0; i < stat->num_outputs; ++i) { free(stat->outputs[i].values); }
	free(stat->outputs);
	free(stat->inputs);
}

void freeStatGraph(StatGraph* graph) {
	int i;
	for (i = 0; i < graph->num_stats; ++i) { freeStat(graph->stats + i); }
	free(graph->stats);
	free(graph);
}