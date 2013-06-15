#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quote.h"
#include "stats.h"

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

int runStat(struct _statGraph_stat* stat, StatGraph* graph) {
	if (stat->outputs) return 0;

	if (stat->stat == 0) {
		#ifdef DEBUG
			printf("\tstat: -1: Copy Multiple Outputs\n");
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

		return 0;
	}

	return 1;
}

int runGraph(StatGraph* graph) {
	return runStat(graph->stats + graph->head, graph);
}

// int getGraphStep(StatGraph* graph) {
// 	int step = 0;
// 	while(graph->steps[step].stats[0].outputs) {
// 		printf("step output:: %p\n", graph->steps[step].stats[0].outputs);
// 		++step;
// 	}
// 	return step;
// }

// struct _statGraph_output* getInput(struct _statGraph_step* source, struct _statGraph_input* map) {
// 	#ifdef DEBUG
// 		printf("source->stats: %p\n", source->stats);
// 	#endif

// 	return source->stats[map->stat].outputs + map->output;
// }

// int executeStat(struct _statGraph_step* last_step, struct _statGraph_stat* stat) {
// 	#ifdef DEBUG
// 		printf("\tStatId: %i\n", stat->stat);
// 	#endif

// 	

// 	return 1;
// }

// int executeGraphStep(StatGraph* graph, int step_pos) {
// 	#ifdef DEBUG
// 		printf("----- start:::executeGraphStep\n");
// 	#endif

// 	struct _statGraph_step* step = graph->steps + step_pos;
// 	struct _statGraph_step* last_step = step_pos == 0? 0 : step - 1;

// 	#ifdef DEBUG
// 		printf("----- start:::stats:::%i\n", step->num_stats);
// 	#endif

// 	int i, err;
// 	for (i = 0; i < step->num_stats; ++i) {
// 		if ((err = executeStat(last_step, step->stats + i))) {
// 			#ifdef DEBUG
// 				printf("----- error:::executeGraphStep: %i\n", err);
// 			#endif

// 			exit(1);
// 			return err;
// 		}
// 	}

// 	#ifdef DEBUG
// 		printf("----- end:::executeGraphStep\n");
// 	#endif

// 	return 0;
// }