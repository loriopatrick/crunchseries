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

void addCalcByNetwork(struct Calc* calc, int sockfd) {
	char* id = malloc(10);
	readNetLen(sockfd, id, 10);

	if (!strncmp(id, "ACD", 3)) {
		double* data = malloc(sizeof(double));
		readNetLen(sockfd, data, sizeof(double));
		addCalcStat(calc, accumulationDistribution, data);
	} else {
		printf("Unknown statistic: %s\n", id);
	}

	free(id);
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

	for (i = 0; i < request.number_of_stats; ++i) {
		addCalcByNetwork(&calc, sockfd);
	}

	char* query = getQuoteSymbolRangeQuery(request.symbol, request.start_epoch, request.end_epoch);
	int items = doCalc(request.series, query, &calc);
	free(query);

	sendNetLen(sockfd, &request.number_of_stats, sizeof(int));
	sendNetLen(sockfd, &items, sizeof(int));

	for (i = 0; i < request.number_of_stats; ++i) {
		int j;
		for (j = 0; j < items; ++j) {
			printf("SEND: %u :: %f\n", calc.results[i][j].epoch, calc.results[i][j].value);
			sendNetLen(sockfd, calc.results[i] + j, sizeof(struct TimePair));
			// printf("Sent\n");
		}
	}
	
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