#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "quote.h"

int QUOTE_get(DB_RES* res, QUOTE* quote) {
	MYSQL_ROW row;
	row = mysql_fetch_row(res);
	if (!row) return 0;
	memcpy(quote->symbol, row[0], 8);
	quote->utime = atol(row[1]);
	quote->high = atof(row[2]);
	quote->low = atof(row[3]);
	quote->open = atof(row[4]);
	quote->close = atof(row[5]);
	quote->volume = atoi(row[6]);
	return 1;
}

void QUOTE_print(QUOTE* quote) {
	printf("%s | %i | %f | %f | %f | %f | %i\n",
		quote->symbol,
		quote->utime,
		quote->high,
		quote->low,
		quote->open,
		quote->close,
		quote->volume
		);
}

char* QUOTE_getQS(char* series, char* query) {
	int series_len = strlen(series), query_len = strlen(query);
	char* res = malloc(67 + series_len + query_len);
	memcpy(res, "SELECT symbol, epoch, high, low, open, close, volume FROM series_", 65);
	memcpy(res + 65, series, series_len);
	memcpy(res + 65 + series_len, " ", 1);
	memcpy(res + 66 + series_len, query, query_len);
	res[66 + series_len + query_len] = '\0';
	return res;
}

char* QUOTE_getQSRange(char* symbol, unsigned int begin, unsigned int end) {
	char* format = " WHERE symbol=\"%s\" AND epoch >= %u AND epoch <= %u ORDER BY epoch ASC\0";
	int query_size = 1 + snprintf(0, 0, format, symbol, begin, end);
	char* query = malloc(query_size);
	memset(query, '\0', query_size);
	sprintf(query, format, symbol, begin, end);

	return query;
}