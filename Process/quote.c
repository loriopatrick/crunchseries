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
	quotes->utime[pos] = atol(row[1]);
	quotes->high[pos] = atof(row[2]);
	quotes->low[pos] = atof(row[3]);
	quotes->open[pos] = atof(row[4]);
	quotes->close[pos] = atof(row[5]);
	quotes->volume[pos] = atoi(row[6]);
	return 1;
}

QUOTES* getQuotes(MYSQL_RES* res) {
	int count = DB_numRows(res);
	if (count <= 0) return 0;

	QUOTES* quotes = malloc(sizeof(QUOTES));
	
	quotes->count = count;
	quotes->utime = malloc(sizeof(unsigned int) * count);
	quotes->high = malloc(sizeof(double) * count);
	quotes->low = malloc(sizeof(double) * count);
	quotes->open = malloc(sizeof(double) * count);
	quotes->close = malloc(sizeof(double) * count);
	quotes->volume = malloc(sizeof(unsigned int) * count);

	int pos = 0;
	while(getQuote(res, quotes, pos++));

	return quotes;
}

void printQuote(QUOTES* quotes, int place) {
	if (place < 0 || place >= quotes->count) {
		printf("-%i out of range-\n", place);
		return;
	}

	printf("%i | %f | %f | %f | %f | %i\n",
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

char* getQuoteQSRange(char* symbol, unsigned int begin, unsigned int end) {
        char* format = " WHERE symbol=\"%s\" AND epoch >= %u AND epoch <= %u ORDER BY epoch ASC\0";
        int query_size = 1 + snprintf(0, 0, format, symbol, begin, end);
        char* query = malloc(query_size);
        memset(query, '\0', query_size);
        sprintf(query, format, symbol, begin, end);

        return query;
}