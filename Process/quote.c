#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "quote.h"

int getQuote(MYSQL_RES* mysql_res, struct Quote* quote) {
	MYSQL_ROW row;
	row = mysql_fetch_row(mysql_res);
	if (!row) return 0;
	memcpy(quote->symbol, row[0], 8);
	quote->epoch = atol(row[1]);
	quote->high = atof(row[2]);
	quote->low = atof(row[3]);
	quote->open = atof(row[4]);
	quote->close = atof(row[5]);
	quote->volume = atoi(row[6]);
	return 1;
}

void printQuote(struct Quote* quote) {
	printf("%s | %i | %f | %f | %f | %f | %i\n",
		quote->symbol,
		quote->epoch,
		quote->high,
		quote->low,
		quote->open,
		quote->close,
		quote->volume
		);
}

char* getQuoteQuery(char* series, char* query) {
	int series_len = strlen(series), query_len = strlen(query);
	char* res = malloc(67 + series_len + query_len);
	memcpy(res, "SELECT symbol, epoch, high, low, open, close, volume FROM series_", 65);
	memcpy(res + 65, series, series_len);
	memcpy(res + 65 + series_len, " ", 1);
	memcpy(res + 66 + series_len, query, query_len);
	res[66 + series_len + query_len] = '\0';
	return res;
}

MYSQL* getConn() {
	mysql_library_init(0, NULL, NULL);
	MYSQL* conn = mysql_init(0);
	mysql_real_connect(conn, "localhost", "root", "root", "crunchseries", 0, 0, 0);
	return conn;
}

MYSQL_RES* requestQuotes(char* query, MYSQL* conn) {
	mysql_query(conn, query);
	return mysql_store_result(conn);
}

void printMYSQLError(MYSQL* conn) {
	printf("MYSQL ERROR: %s\n", mysql_error(conn));
}