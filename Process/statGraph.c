#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "statGraph.h"

// #define DEBUG

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

struct graph* buildGraph(void** inputs, void* data, int len) {
	struct graph* res = malloc(sizeof(struct graph));
	res->inputs = inputs;

	int intSize = sizeof(int);

	int pos = 0, i, j, k;
	memcpy(&res->steps_len, data, intSize); pos += intSize;
	#ifdef DEBUG
	printf("# of steps: %i\n", res->steps_len);
	#endif

	res->steps = malloc(sizeof(struct step) * res->steps_len);

	for (i = 0; i < res->steps_len; ++i) { // {foreach step}
		struct step* step = res->steps + i;
		
		memcpy(&step->stats_len, data + pos, intSize); pos += intSize; // number of stats for step
		
		#ifdef DEBUG
		printf("\t# of stats: %i\n", step->stats_len);
		#endif

		step->stats = malloc(sizeof(int) * step->stats_len);
		step->settings = malloc(sizeof(void*) * step->stats_len);
		step->setting_sizes = malloc(sizeof(int*) * step->stats_len);
		step->settings_len = malloc(sizeof(int) * step->stats_len);
		step->input_maps = malloc(sizeof(int*) * step->stats_len);
		step->outputs = 0;
		
		for (j = 0; j < res->steps[i].stats_len; ++j) { // {foreach stat}
			memcpy(step->stats + j, data + pos, intSize); pos += intSize; // stat id number
			
			#ifdef DEBUG
			printf("\t\tstat id: %i\n", step->stats[j]);
			#endif

			memcpy(step->settings_len + j, data + pos, intSize); // number of settings for stat
			
			#ifdef DEBUG
			printf("\t\t# of settings: %i\n", step->settings_len[j]);
			#endif

			step->setting_sizes[j] = malloc(intSize * step->settings_len[j]);
			step->settings[j] = malloc(sizeof(void*) * step->settings_len[j]);
			pos += intSize;

			for (k = 0; k < step->settings_len[j]; ++k) { // {foreach setting}
				memcpy(step->setting_sizes[j] + k, data + pos, intSize); pos += intSize;
				
				#ifdef DEBUG
				printf("\t\t\tsetting size: %i\n", step->setting_sizes[j][k]);
				#endif

				step->settings[j][k] = malloc(step->setting_sizes[j][k]);
				memcpy(step->settings[j][k], data + pos, step->setting_sizes[j][k]); pos += step->setting_sizes[j][k];
				
				#ifdef DEBUG
				printf("\t\t\tsetting value: %s\n", step->settings[j][k]);
				#endif
			}

			int input_maps_len;
			memcpy(&input_maps_len, data + pos, intSize); pos += intSize;
			
			#ifdef DEBUG
			printf("\t\t# of inptus: %i\n", input_maps_len);
			#endif
			
			if (input_maps_len > 0) {
				step->input_maps[j] = malloc(intSize * input_maps_len);
				memcpy(step->input_maps + j, data + pos, intSize * input_maps_len); pos += intSize * input_maps_len;
			}
		}
	}

	return res;
}

int getGraphStep(struct graph* graph) {
	int step = 0;
	while(graph->steps[step].outputs) ++step;
	return step;
}

double* executeStat(void** inputs, int* input_map, int stat) {
	return 0;
}

void executeGraphStep(struct graph* graph, int step_pos) {
	void** last_output = step_pos == 0? 
		graph->inputs : 
		(void**)graph->steps[step_pos - 1].outputs;

	int i;
	struct step* step = graph->steps + step_pos;
	step->outputs = malloc(sizeof(void*) * step->stats_len);
	for (i = 0; i < step->stats_len; ++i) {
		int* input_map = step->input_maps[i];
		step->outputs[i] = executeStat(last_output, input_map, step->stats[i]);
	}
}