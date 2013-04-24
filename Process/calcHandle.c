#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "calc.h"
#include "stats.h"
#include "server.h"

#include "calcHandle.h"

struct CalcStatRequest {
	char symbol[9];
	char series[4];
	unsigned int start_epoch;
	unsigned int end_epoch;
	int number_of_stats;
};

char* addCalcByNetwork(CalcStream* calc, int sockfd) {
	char* id = malloc(10);
	readNetLen(sockfd, id, 10);

	printf("Add: %s\n", id);

	if (!strncmp(id, "ACD", 3)) {
		double start;
		readNetLen(sockfd, &start, sizeof(double));
		void* mem = STREAMSTAT_accumulationDistribution_mem(start);
		addCalcStat(calc, STREAMSTAT_accumulationDistribution, mem);
	} else if (!strncmp(id, "AROON_UP", 8)) {
		double tail_size;
		readNetLen(sockfd, &tail_size, sizeof(int));
		void* mem = STREAMSTAT_aroon_mem(tail_size);
		addCalcStat(calc, STREAMSTAT_aroonUp, mem);
	} else if (!strncmp(id, "AROON_DOWN", 10)) {
		double tail_size;
		readNetLen(sockfd, &tail_size, sizeof(int));
		void* mem = STREAMSTAT_aroon_mem(tail_size);
		addCalcStat(calc, STREAMSTAT_aroonDown, mem);
	} else if (!strncmp(id, "SMA", 3)) {
		double tail_size;
		readNetLen(sockfd, &tail_size, sizeof(int));
		void* mem = STREAMSTAT_movingAverage_mem(tail_size);
		addCalcStat(calc, STREAMSTAT_movingAverageSimple, mem);
	} else if (!strncmp(id, "STDV", 4)) {
		double tail_size;
		readNetLen(sockfd, &tail_size, sizeof(int));
		void* mem = STREAMSTAT_standardDeviation_mem(tail_size);
		addCalcStat(calc, STREAMSTAT_standardDeviation, mem);
	} else {
		printf("Unknown statistic: %s\n", id);
		free(id);
		return 0;
	}

	return id;
}

void statCalcHandle(int sockfd) {
	int i;

	struct CalcStatRequest request;
	readNetLen(sockfd, &request, sizeof(struct CalcStatRequest));
	request.symbol[8] = '\0';
	request.series[3] = '\0';

	printf("symbol: %s\nseries: %s\nstart: %u\nend: %u\nstats: %i\n\n", request.symbol, request.series, request.start_epoch, request.end_epoch, request.number_of_stats);

	struct Calc calc;
	initCalc(&calc);

	char** stat_names = malloc(sizeof(char*) * request.number_of_stats);

	for (i = 0; i < request.number_of_stats; ++i) {
		stat_names[i] = addCalcByNetwork(&calc, sockfd);
	}

	char* query = getQuoteSymbolRangeQuery(request.symbol, request.start_epoch, request.end_epoch);
	int items = doCalc(request.series, query, &calc);
	free(query);

	printf("Done with calc\n");

	sendNetLen(sockfd, &request.number_of_stats, sizeof(int));
	sendNetLen(sockfd, &items, sizeof(int));

	for (i = 0; i < request.number_of_stats; ++i) {
		char* name = stat_names[i];
		if (!name) continue;
		sendNetLen(sockfd, name, 10);
		int j;
		for (j = 0; j < items; ++j) {
			// printf("SEND: %u :: %f\n", calc.results[i][j].epoch, calc.results[i][j].value);
			sendNetLen(sockfd, calc.results[i] + j, sizeof(struct TimePair));
			// printf("Sent\n");
		}
		free(name);
	}

	free(stat_names);
	freeCalc(&calc);
}


void calcHandle(int sockfd) {
	int calc_type;
	
	readNetLen(sockfd, &calc_type, sizeof(int));

	if (calc_type == 1) {
		statCalcHandle(sockfd);
		return;
	}
}