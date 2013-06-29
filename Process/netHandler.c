#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "netHandler.h"

#include "quote.h"
#include "network.h"
#include "statGraph.h"

void sendArray(int sockfd, double* array, int len) {
	NET_send(sockfd, &len, sizeof(int));

	int i;
	for (i = 0; i < len; ++i) {
		NET_send(sockfd, array + i, sizeof(double));
	}
}

void netHandler(int sockfd) {
	printf("%s\n", "New Connection");
	int requestType;
	NET_recv(sockfd, &requestType, sizeof(int));

	if (requestType != 1) {
		return;
	}

	int stat_graph_size;
	NET_recv(sockfd, &stat_graph_size, sizeof(int));

	char* stat_graph_data = malloc(stat_graph_size);
	NET_recv(sockfd, stat_graph_data, stat_graph_size);

	StatGraph* graph = buildGraph(stat_graph_data);
	runGraph(graph);

	struct _statGraph_stat output_stat = graph->stats[graph->head];

	int number_of_outputs = output_stat.num_inputs;
	printf("NUmber of outputs: %i\n", number_of_outputs);
	NET_send(sockfd, &number_of_outputs, sizeof(int));

	int i;
	for (i = 0; i < number_of_outputs; ++i) {
		struct _statGraph_output output = output_stat.outputs[i];
		sendArray(sockfd, output.values, output.len);
	}
}