#ifndef __QUOTE_H__
#define __QUOTE_H__

#include <mysql.h>

struct _Quote {
	char symbol[8];
	unsigned int time;
	double high;
	double low;
	double open;
	double close;
	unsigned int volume;
} typedef Quote;

struct _TimeValue {
	unsigned int time;
	double value;
};

void printQuote(Quote* quote);
char* getQuoteQuery(char* series, char* query);
char* getQuoteSymbolRangeQuery(char* symbol, unsigned int start, unsigned int end);

#endif