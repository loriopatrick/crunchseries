#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "database.h"
#include "quote.h"

int getQuote(MYSQL_RES* res, QUOTES* quotes, int pos) {
	MYSQL_ROW row;
	row = mysql_fetch_row(res);
	if (!row) return 0;
	quotes->utime[pos] = atof(row[1]);
	quotes->high[pos] = atof(row[2]);
	quotes->low[pos] = atof(row[3]);
	quotes->open[pos] = atof(row[4]);
	quotes->close[pos] = atof(row[5]);
	quotes->volume[pos] = atof(row[6]);
	return 1;
}

QUOTES* getQuotes(MYSQL_RES* res) {
	int count = mysql_num_rows(res);
	// if (count <= 0) return 0;

	QUOTES* quotes = malloc(sizeof(QUOTES));
	
	quotes->count = count;
	quotes->utime = malloc(sizeof(double) * count);
	quotes->high = malloc(sizeof(double) * count);
	quotes->low = malloc(sizeof(double) * count);
	quotes->open = malloc(sizeof(double) * count);
	quotes->close = malloc(sizeof(double) * count);
	quotes->volume = malloc(sizeof(double) * count);

	int pos = 0;
	while(getQuote(res, quotes, pos++));
	
	return quotes;
}

QUOTES* getQuotesByQuery(char* query) {
	MYSQL_RES* res = DB_query(query);
	QUOTES* quotes = getQuotes(res);
	mysql_free_result(res);
	return quotes;
}

void printQuote(QUOTES* quotes, int place) {
	if (place < 0 || place >= quotes->count) {
		printf("-%i out of range-\n", place);
		return;
	}

	printf("%f | %f | %f | %f | %f | %f\n",
		quotes->utime[place],
		quotes->high[place],
		quotes->low[place],
		quotes->open[place],
		quotes->close[place],
		quotes->volume[place]
		);
}

char* getQuoteQS(char* series, char* query) {
	int series_len = strlen(series), query_len = strlen(query);
	char* res = malloc(67 + series_len + query_len);
	memcpy(res, "SELECT symbol, epoch, high, low, open, close, volume FROM series_", 65);
	memcpy(res + 65, series, series_len);
	memcpy(res + 65 + series_len, " ", 1);
	memcpy(res + 66 + series_len, query, query_len);
	res[66 + series_len + query_len] = '\0';
	return res;
}

char* getQuoteQSRange(char* symbol, double begin, double end) {
	char* format = " WHERE symbol=\"%s\" AND epoch >= %f AND epoch <= %f ORDER BY epoch ASC\0";
	char* query = 0;
	asprintf(&query, format, symbol, begin, end);
	return query;
}