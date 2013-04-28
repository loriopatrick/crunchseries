#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "network.h"
#include "database.h"
#include "qStreamCalcNetHandler.h"
#include "qStreamStats.h"
#include "qStreamCalc.h"

void test_calc() {
	QSTREAM_CALC calc;
	QSTREAM_CALC_init(&calc);

	void* mem;
	mem = QSTREAM_STAT_percentB_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_percentB, mem, 0);

	mem = QSTREAM_STAT_accumulationDistribution_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_accumulationDistribution, mem, 0);

	mem = QSTREAM_STAT_aroon_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_aroonUp, mem, 0);

	mem = QSTREAM_STAT_standardDeviation_mem(20);
	QSTREAM_CALC_addStat(&calc, QSTREAM_STAT_standardDeviation, mem, 0);

	QSTREAM_CALC_do(&calc, "eom", "ORDER BY epoch ASC");
	QSTREAM_CALC_free(&calc);
}

void test_server() {
	NET_startServer(4213, 20, QSTREAM_CALC_netHandler);
}

int main (int args, char** argv) {
	DB_connect();

	// test_getQuote();

	int i;
	for (i = 0; i < 100; ++i) {
		struct timeval start, end;
		gettimeofday(&start, 0);

		test_calc();

		gettimeofday(&end, 0);
		printf("%.5g seconds\n", end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec));
	}

	// test_server();

	DB_close();
	return 0;
}