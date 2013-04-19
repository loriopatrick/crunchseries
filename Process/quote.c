#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "quote.h"

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