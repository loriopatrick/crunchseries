#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "quote.h"
#include "qStreamCalc.h"
#include "qStreamStats.h"
#include "network.h"

struct requestHeader {
	char symbol[9];
	char series[4];
	unsigned int begin_utime;
	unsigned int end_utime;
	int number_of_stats;
};

char* addStatRequest(QSTREAM_CALC* calc, int sockfd) {
	char* name = malloc(10);
	char* id = malloc(3);

	NET_recv(sockfd, id, 3);	
	NET_recv(sockfd, name, 10);

	printf("Add: %s\n", name);

	if (!strncmp(name, "ACD", 3)) {
		double start;
		NET_recv(sockfd, &start, sizeof(double));
		void* mem = QSTREAM_STAT_accumulationDistribution_mem(start);
		QSTREAM_CALC_addStat(calc, QSTREAM_STAT_accumulationDistribution, mem, 0);
	} else if (!strncmp(name, "AROON_UP", 8)) {
		int tail_size;
		NET_recv(sockfd, &tail_size, sizeof(int));
		void* mem = QSTREAM_STAT_aroon_mem(tail_size);
		QSTREAM_CALC_addStat(calc, QSTREAM_STAT_aroonUp, mem, 0);
	} else if (!strncmp(name, "AROON_DOWN", 10)) {
		int tail_size;
		NET_recv(sockfd, &tail_size, sizeof(int));
		void* mem = QSTREAM_STAT_aroon_mem(tail_size);
		QSTREAM_CALC_addStat(calc, QSTREAM_STAT_aroonDown, mem, 0);
	} else if (!strncmp(name, "SMA", 3)) {
		int tail_size;
		NET_recv(sockfd, &tail_size, sizeof(int));
		void* mem = QSTREAM_STAT_movingAverage_mem(tail_size);
		QSTREAM_CALC_addStat(calc, QSTREAM_STAT_movingAverageSimple, mem, 0);
	} else if (!strncmp(name, "STDV", 4)) {
		int tail_size;
		NET_recv(sockfd, &tail_size, sizeof(int));
		void* mem = QSTREAM_STAT_standardDeviation_mem(tail_size);
		QSTREAM_CALC_addStat(calc, QSTREAM_STAT_standardDeviation, mem, 0);
	} else if (!strncmp(name, "PB", 2)) {
		int tail_size;
		NET_recv(sockfd, &tail_size, sizeof(int));
		void* mem = QSTREAM_STAT_percentB_mem(tail_size);
		QSTREAM_CALC_addStat(calc, QSTREAM_STAT_percentB, mem, 0);
	} else {
		printf("Unknown statistic: %s\n", name);
		free(id);
		return 0;
	}

	free(name);
	return id;
}

void QSTREAM_CALC_netHandler(int sockfd) {
	int i;

	struct requestHeader request;
	NET_recv(sockfd, &request, sizeof(struct requestHeader));
	request.symbol[8] = '\0';
	request.series[3] = '\0';

	printf("symbol: %s\nseries: %s\nstart: %u\nend: %u\nstats: %i\n\n", request.symbol, request.series, request.begin_utime, request.end_utime, request.number_of_stats);

	QSTREAM_CALC calc;
	QSTREAM_CALC_init(&calc);

	char** stat_ids = malloc(sizeof(char*) * request.number_of_stats);

	for (i = 0; i < request.number_of_stats; ++i) {
		stat_ids[i] = addStatRequest(&calc, sockfd);
	}

	char* query = QUOTE_getQSRange(request.symbol, request.begin_utime, request.end_utime);
	int items = QSTREAM_CALC_do(&calc, request.series, query);
	free(query);

	printf("Done with calc\n");

	NET_send(sockfd, &request.number_of_stats, sizeof(int));
	NET_send(sockfd, &items, sizeof(int));

	for (i = 0; i < request.number_of_stats; ++i) {
		char* id = stat_ids[i];
		if (!id) continue;
		NET_send(sockfd, id, 3);
		int j;
		for (j = 0; j < items; ++j) {
			// printf("SEND: %u :: %f\n", calc.results[i][j].epoch, calc.results[i][j].value);
			NET_send(sockfd, calc.results[i] + j, sizeof(TIMEVALUE));
			// printf("Sent\n");
		}
		free(id);
	}

	free(stat_ids);
	QSTREAM_CALC_free(&calc);
}