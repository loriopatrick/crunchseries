#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <mysql.h>

#include "quote.h"
#include "calc.h"
#include "server.h"
#include "stats.h"
#include "calcHandle.h"

void test_getQuote(MYSQL* conn) {
	char* query = getQuoteQuery("eom", "ORDER BY epoch DESC");
	mysql_query(conn, query);
	free(query);

	MYSQL_RES* results = mysql_store_result(conn);
	struct Quote quote;

	while(getQuote(results, &quote)) {
		printQuote(&quote);
	}
}

void get_open(double* result, struct Quote* quote, void* state) {
	(*result) = quote->close;
}

void test_calc(MYSQL* conn) {
	struct Calc calc;
	initCalc(&calc);

	double* mem = malloc(sizeof(double));
	*mem = 0;
	addCalcStat(&calc, accumulationDistribution, mem);
	
	doCalc("eom", "ORDER BY epoch DESC", &calc, conn);
	freeCalc(&calc);
}

void test_server() {
	startServer(4213, 20, calcHandle);
}

int main (int args, char** argv) {
	MYSQL* conn = getConn();
	// int i;
	// for (i = 0; i < 100; ++i) {
	// 	struct timeval start, end;
	// 	gettimeofday(&start, 0);

	// 	test_calc(conn);

	// 	gettimeofday(&end, 0);
	// 	printf("%.5g seconds\n", end.tv_sec - start.tv_sec + 1E-6 * (end.tv_usec - start.tv_usec));
	// }

	test_server();

	// printf("CLOSE MYSQL!!\n");
	mysql_close(conn);
	return 0;
}