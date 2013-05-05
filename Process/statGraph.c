#include <stdlib.h>
#include <string.h>

#include "statGraph.h"


/*
	Data format
	- number of steps
	- {foreach step}
		- number of stats
		- {foreach stat}
			- stat number
			- number of inputs for stat
			- input map for stat

*/

struct graph* buildGraph(void** inputs, void* data, int len) {
	struct graph* res = malloc(sizeof(struct graph));
	res->inputs = inputs;

	int intSize = sizeof(int);

	int pos = 0, i, j;
	memcpy(&res->steps_len, data, intSize);
	res->steps = malloc(sizeof(struct step) * res->steps_len);
	pos += intSize;

	for (i = 0; i < res->steps_len; ++i) {
		struct step* step = res->steps + i;
		
		memcpy(&step->stats_len, data + pos, intSize);
		step->stats = malloc(sizeof(int) * step->stats_len);
		step->input_maps = malloc(sizeof(int*) * step->stats_len);
		step->outputs = 0;
		pos += intSize;
		
		for (j = 0; j < res->steps[i].stats_len; ++j) {
			memcpy(step->stats + j, data + pos, intSize);
			pos += intSize;

			int input_maps_len;
			memcpy(&input_maps_len, data + pos, intSize);
			pos += intSize;

			step->input_maps[j] = malloc(intSize * input_maps_len);
			memcpy(step->input_maps[j], data + pos, intSize * input_maps_len);
			pos += intSize * input_maps_len;
		}
	}

	return res;
}

int getGraphStep(struct graph* graph) {
	int step = 0;
	while(graph->steps[step].outputs) ++step;
	return step;
}

void* executeStat(void** inputs, int* input_map, int stat) {
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