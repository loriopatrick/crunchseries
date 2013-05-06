#ifndef __CRUNCHSERIES_PROCESS_STATGRAPH_H__
#define __CRUNCHSERIES_PROCESS_STATGRAPH_H__

struct step {
	int** input_maps;
	int* setting_sizes;
	void** settings;
	int settings_len;
	int* stats;
	int stats_len;
	double** outputs;
};

struct graph {
	void** inputs;
	int steps_len;
	struct step* steps;
};

struct graph* buildGraph(void** inputs, void* data, int len);
int getGraphStep(struct graph* graph);
double* executeStat(void** inputs, int* input_map, int stat);
void executeGraphStep(struct graph* graph, int step_pos);

#endif