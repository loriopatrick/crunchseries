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

struct _statGraph_step {
	struct _statGraph_stat* stats;
	int num_stats;
};

struct _statGraph_graph {
	struct _statGraph_step* steps;
	int num_steps;
} typedef StatGraph;

StatGraph* buildGraph(void* data);
int getGraphStep(StatGraph* graph);
int executeGraphStep(StatGraph* graph, int step_pos);

#endif