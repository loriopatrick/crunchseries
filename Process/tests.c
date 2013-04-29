#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "network.h"
#include "database.h"
#include "qStreamCalcNetHandler.h"
#include "qStreamStats.h"
#include "streamStats.h"
#include "qStreamCalc.h"

void test_calc() {
	QSTREAM_CALC calc;
	QSTREAM_CALC_init(&calc);

	void* mem;
	mem = STREAM_STAT_percentB_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_percentB, mem, STREAM_STAT_percentB_mem_free);

	mem = QSTREAM_STAT_accumulationDistribution_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_accumulationDistribution, mem, 0);

	mem = STREAM_STAT_movingAverageExponetial_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_movingAverageExponetial, mem, 0);

	mem = QSTREAM_STAT_aroon_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_aroonUp, mem, QSTREAM_STAT_aroon_mem_free);

	mem = STREAM_STAT_standardDeviation_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_standardDeviation, mem, STREAM_STAT_standardDeviation_mem_free);

	QSTREAM_CALC_do(&calc, "eom", "ORDER BY epoch ASC");
	QSTREAM_CALC_free(&calc);
}

void test_stats() {
	double data[] = {100, 43, 2, 2, 3, 3, 2, 434, 54, 3, 2, 4, 5};
	// 100 : 0
	// 100, 43 : 1
	// 100, 43, 2 : 2
	// 100, 43, 2, 2, : 3
	// 100, 43, 2, 2, 3 : 4
	// 43, 2, 2, 3, 3 : 4
	// 2, 2, 3, 3, 2 : 1
	// 2, 3, 3, 2, 434 : 0
	// 3, 3, 2, 434, 54 : 1
	// 3, 2, 434, 54, 3 : 2
	// 2, 434, 54, 3, 2 : 3
	// 434, 54, 3, 2, 4 : 4
	// 54, 3, 2, 4, 5 : 4


	// expected: 0, 1, 2, 3, 4, 4, 1, 0, 1, 2, 3, 4, 4
	void* mem = STREAM_STAT_lastThres_mem(5, 1, 1);

	double res;
	int i = 0, len = 13;
	for (; i < len; ++i) {
		STREAM_STAT_lastThres(&res, data + i, mem);
		printf("%f\n", res);
	}
}

void test_server() {
	NET_startServer(4213, 20, QSTREAM_CALC_netHandler);
}

int main (int args, char** argv) {
	DB_connect();

	// test_getQuote();

	test_stats();

	// int i;
	// for (i = 0; i < 100; ++i) {
	// 	struct timeval start, end;
	// 	gettimeofday(&start, 0);

	// 	test_calc();

	// 	gettimeofday(&end, 0);
	// 	printf("%.5g seconds\n", end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec));
	// }

	// test_server();

	DB_close();
	return 0;
}