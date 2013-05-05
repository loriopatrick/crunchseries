#ifndef __CRUNCHSERIES_PROCESS_STATGRAPH_H__
#define __CRUNCHSERIES_PROCESS_STATGRAPH_H__

struct step {
	int** input_maps;
	int* stats;
	int stats_len;
	double** outputs;
};

struct graph {
	void** inputs;
	int steps_len;
	struct step* steps;
};

#endif