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
			- number of outputs for stat
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

	memcpy(&stat->num_outputs, data + pos, sizeof(int)); pos += sizeof(int);
	#ifdef DEBUG
		printf("\tnumber of outputs: %i\n", stat->num_outputs);
	#endif

	stat->outputs = 0;
	return pos;
}

StatGraph* buildGraph(void* data) {
	#ifdef DEBUG
		printf("----- start:::buildGraph\n");
	#endif

	StatGraph* res = malloc(sizeof(StatGraph));

	int pos = 0;
	memcpy(&res->num_steps, data+pos, sizeof(int)); pos += sizeof(int);

	#ifdef DEBUG
		printf("# of steps: %i\n", res->num_steps);
	#endif

	res->steps = malloc(sizeof(struct _statGraph_step));

	int i, j;
	for (i = 0; i < res->num_steps; ++i) {
		memcpy(&res->steps[i].num_stats, data + pos, sizeof(int)); pos += sizeof(int);
		res->steps[i].stats = malloc(sizeof(struct _statGraph_stat));

		for (j = 0; j < res->steps[i].num_stats; ++j) {
			pos += buildStat(res->steps[i].stats + j, data + pos);
			#ifdef DEBUG
				printf("\n");
			#endif
		}
	}

	#ifdef DEBUG
		printf("----- end:::buildGraph\n");
	#endif

	return res;
}

int getGraphStep(StatGraph* graph) {
	int step = 0;
	while(graph->steps[step].stats[0].outputs) ++step;
	return step;
}

struct _statGraph_output* getInput(struct _statGraph_step* source, struct _statGraph_input* map) {
	return source->stats[map->stat].outputs + map->output;
}

int executeStat(struct _statGraph_step* last_step, struct _statGraph_stat* stat) {
	if (stat->stat == 1) { // sql query
		#ifdef DEBUG
			printf("\tstat: 1: SQL Query\n");
		#endif

		char* query = stat->settings[0].data;
		QUOTES* quotes = getQuotesByQuery(query);
		printf("\tquery: %s\n\t# of results: %i\n", query, quotes->count);

		stat->outputs = malloc(sizeof(struct _statGraph_output) * stat->num_outputs);
		stat->outputs[0].values = quotes->utime; stat->outputs[0].len = quotes->count;
		stat->outputs[1].values = quotes->high; stat->outputs[1].len = quotes->count;
		stat->outputs[2].values = quotes->low; stat->outputs[2].len = quotes->count;
		stat->outputs[3].values = quotes->open; stat->outputs[3].len = quotes->count;
		stat->outputs[4].values = quotes->close; stat->outputs[4].len = quotes->count;
		stat->outputs[5].values = quotes->volume; stat->outputs[5].len = quotes->count;

		return 0;
	}

	if (stat->stat == 2) {
		#ifdef DEBUG
			printf("\tstat: 2: Simple Moving Average\n");
		#endif

		int* period_size = stat->settings[0].data;
		struct _statGraph_output* series = getInput(last_step, stat->inputs);

		double* res = simpleMovingAverage(series->values, series->len, *period_size);
		stat->outputs = malloc(sizeof(struct _statGraph_output) * stat->num_outputs);
		stat->outputs[0].values = res; stat->outputs[0].len = series->len;
	}

	return 1;
}

int executeGraphStep(StatGraph* graph, int step_pos) {
	#ifdef DEBUG
		printf("----- start:::executeGraphStep\n");
	#endif

	struct _statGraph_step* step = graph->steps + step_pos;
	struct _statGraph_step* last_step = step_pos == 0? 0 : step - 1;

	int i, err;
	for (i = 0; i < step->num_stats; ++i) {
		if ((err = executeStat(last_step, step->stats + i))) {
			#ifdef DEBUG
				printf("----- error:::executeGraphStep: %i\n", err);
			#endif

			return err;
		}
	}

	#ifdef DEBUG
		printf("----- end:::executeGraphStep\n");
	#endif
	return 0;
}