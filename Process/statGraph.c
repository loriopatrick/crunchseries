#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quote.h"

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

int executeStat(struct _statGraph_step* last_step, struct _statGraph_stat* stat, double** outputs) {
	// struct outDump* out = malloc(sizeof(struct outDump));

	// if (stat == 1) { // sql query
	// 	char* query = settings[0];
	// 	char* queryFixed = malloc(settings_sizes[0] + 1);
	// 	memcpy(queryFixed, query, settings_sizes[0]);
	// 	queryFixed[settings_sizes[0]] = '\0';
	// 	free(query);
	// 	QUOTES* quotes = getQuotesByQuery(queryFixed);
	// 	free(queryFixed);

	// 	out->outputs = malloc(sizeof(double*) * 6);
	// 	out->outputs[0] = quotes->utime;
	// 	out->outputs[1] = quotes->high;
	// 	out->outputs[2] = quotes->low;
	// 	out->outputs[3] = quotes->open;
	// 	out->outputs[4] = quotes->close;
	// 	out->outputs[5] = quotes->volume;

	// 	out->outputs_len = 6;
	// 	out->output_size = quotes->count;

	// 	return out;
	// }

	return 0;
}

int executeGraphStep(StatGraph* graph, int step_pos) {

	struct _statGraph_step* step = graph->steps + step_pos;
	struct _statGraph_step* last_step = step_pos == 0? 0 : step - 1;

	int i, err;
	for (i = 0; i < step->num_stats; ++i) {
		double** outputs = malloc(sizeof(double*) * step->num_outputs);
		if ((err = executeStat(last_step, step->stats + i, outputs))) {
			return err;
		}
	}

	return 0;
}