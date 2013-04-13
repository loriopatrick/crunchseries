#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "quote.h"
#include "calc.h"
#include "server.h"
#include "stats.h"

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
	char* query = getQuoteQuery("eom", "ORDER BY epoch DESC");
	mysql_query(conn, query);
	free(query);
	MYSQL_RES* results = mysql_store_result(conn);
	int items = mysql_num_rows(results);




	struct calc calc;
	initCalc(&calc);
	
	double mem = 0;
	addCalcStat(&calc, accumulationDistribution, &mem);

	executeCalc(results, items, &calc);
}

void server_handler(int sockfd) {
	printf("GOT SOCK: %i\n", sockfd);
}

void test_server() {
	startServer(5432, 20, server_handler);
}

int main (int args, char** argv) {
	// test_server();
	MYSQL* conn = mysql_init(0);
	mysql_real_connect(conn, "localhost", "root", "root", "crunchseries", 0, 0, 0);

	test_calc(conn);
	// test_getQuote(conn);

	mysql_close(conn);
}