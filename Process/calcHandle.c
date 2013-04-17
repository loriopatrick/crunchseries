#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "calc.h"
#include "stats.h"

void handleAddCalcStat(int sockfd, struct calc* calc) {
	int size;
	char stat_name[10];
	size = recv(sockfd, stat_name, 10, MSG_WAITALL);
	
	if (size != 10) {
		perror("handleAddCalc stat_name recv");
		exit(1);
	}

	if (!strncmp(stat_name, "ACD", 3)) {
		printf("adding ACD!\n");
		double* data = malloc(sizeof(double));
		size = recv(sockfd, data, sizeof(double), MSG_WAITALL);
		if (size != sizeof(double)) {
			perror("handleAddCalc ACD");
			exit(1);
		}
		addCalcStat(calc, accumulationDistribution, data);
	}
}

struct calcStatRequest {
	char symbol[9];
	char series[4];
	unsigned int start_epoch;
	unsigned int end_epoch;
	int number_of_stats;
};


void calcHandle(int sockfd) {
	printf("%s\n", "CALC HANDLER!!");
	int calc_type, size;
	
	size = recv(sockfd, &calc_type, sizeof(int), MSG_WAITALL);
	if (size != sizeof(int)) {
		perror("calcHandle calc_type");
		exit(1);
	}

	printf("CALC TYPE: %i\n", calc_type);

	if (calc_type == 1) {
		struct calc calc;

		struct calcStatRequest request;
		int r = 0;
		while(r < sizeof(struct calcStatRequest)) {
			size = recv(sockfd, &request + r, sizeof(struct calcStatRequest) - r, 0);
			if (!size) {
				perror("calcHandle calcStatRequest");
				exit(1);
			}
			r += size;
		}
		request.symbol[8] = '\0';
		request.series[3] = '\0';

		printf("symbol: %s\nseries: %s\nstart: %i\nend: %i\nstats: %i\n", request.symbol, request.series, request.start_epoch, request.end_epoch, request.number_of_stats);

		

		int i;
		for (i = 0; i < request.number_of_stats; ++i) {
			handleAddCalcStat(sockfd, &calc);
		}

		
		char* format = " WHERE symbol=\"%s\" AND epoch >= %u AND epoch <= %u ORDER BY epoch ASC";
		int query_size = snprintf(0, 0, format, request.symbol, request.start_epoch, request.end_epoch);
		char* query = malloc(query_size);
		memset(query, '\0', query_size);
		sprintf(query, format, request.symbol, request.start_epoch, request.end_epoch);

		
		doCalc(request.series, query, &calc);

		
		free(query);
		freeCalc(&calc);
	}
}