#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "quote.h"

void printQuote(Quote* quote) {
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

char* getQuoteSymbolRangeQuery(char* symbol, unsigned int start, unsigned int end) {
	char* format = " WHERE symbol=\"%s\" AND epoch >= %u AND epoch <= %u ORDER BY epoch ASC\0";
	int query_size = 1 + snprintf(0, 0, format, symbol, start, end);
	char* query = malloc(query_size);
	memset(query, '\0', query_size);
	sprintf(query, format, symbol, start, end);

	return query;
}