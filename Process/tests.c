#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void test_calc() {
	char* query = getQuoteQuery("eom", "ORDER BY epoch DESC");
	MYSQL_RES* results = requestQuotes(query);
	free(query);

	struct calc calc;
	initCalc(&calc);
	
	double mem = 0;
	addCalcStat(&calc, accumulationDistribution, &mem);

	executeCalc(results, -1, &calc);
}

void test_server() {
	startServer(5432, 20, calcHandle);
}

int main (int args, char** argv) {
	// test_calc();
	test_server();
	closeConnections();
}