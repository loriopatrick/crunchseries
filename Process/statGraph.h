#ifndef __CRUNCHSERIES_PROCESS_STATGRAPH_H__
#define __CRUNCHSERIES_PROCESS_STATGRAPH_H__

struct _statGraph_setting {
	void* data;
	int len;
};

struct _statGraph_output {
	double* values;
	int len;
};

struct _statGraph_input {
	int stat;
	int output;
};

struct _statGraph_stat {
	int stat;

	struct _statGraph_setting* settings;
	int num_settings;

	struct _statGraph_input* inputs;
	int num_inputs;

	struct _statGraph_output* outputs;
	int num_outputs;
};

struct _statGraph_graph {
	struct _statGraph_stat* stats;
	int num_stats;

	int head;
} typedef StatGraph;

StatGraph* buildGraph(void* data);
int runGraph(StatGraph* graph);
int runStat(struct _statGraph_stat* stat, StatGraph* graph);
void freeStatGraph(StatGraph* graph);

#endif