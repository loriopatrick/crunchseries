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

	int number_of_stats = 1;

	double** calc_results = malloc(sizeof(double*) * number_of_stats);
	int i;
	for (i = 0; i < number_of_stats; ++i) {
		calc_results[i] = malloc(sizeof(double) * items);
	}

	void** states = malloc(sizeof(void*) * number_of_stats);
	states[0] = malloc(sizeof(struct aroon));
	struct aroon aroon;
	memset(&aroon, 0, sizeof(struct aroon));
	aroon.tail.tail_size = 20;
	states[0] = (void*)&aroon;

	calc(results, items, number_of_stats, calc_results, &aroonUp, states);
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