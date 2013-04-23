#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "quote.h"
#include "calc.h"
#include "server.h"
#include "stats.h"
#include "calcHandle.h"
#include "database.h"

void test_getQuote() {
	char* query = getQuoteQuery("eom", "ORDER BY epoch DESC");
	DBRes* res = queryDB(query);
	free(query);

	struct Quote quote;

	while(getQuote(res, &quote)) {
		printQuote(&quote);
	}
}

void test_calc() {
	struct Calc calc;
	initCalc(&calc);

	struct standardDeviation* mem = malloc(sizeof(struct standardDeviation));
	memset(mem, 0, sizeof(struct standardDeviation));
	addCalcStat(&calc, standardDeviation, mem);

	mem->s1 = 0;
	mem->s2 = 0;
	mem->tail.tail_size = 23;
	// mem->tail.values = malloc(sizeof(double) * mem->tail.tail_size);

	doCalc("eom", "ORDER BY epoch ASC", &calc);
	freeCalc(&calc);
}

void test_server() {
	startServer(4213, 20, calcHandle);
}

int main (int args, char** argv) {
	connectDB();

	// test_getQuote();

	// int i;
	// for (i = 0; i < 100; ++i) {
	// 	struct timeval start, end;
	// 	gettimeofday(&start, 0);

	// 	test_calc();

	// 	gettimeofday(&end, 0);
	// 	printf("%.5g seconds\n", end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec));
	// }

	test_server();

	closeDB();
	return 0;
}