#ifndef __QUOTE_H__
#define __QUOTE_H__

#include <mysql.h>

struct Quote {
	char symbol[8];
	unsigned int epoch;
	double high;
	double low;
	double open;
	double close;
	unsigned int volume;
};

struct TimePair {
	unsigned int epoch;
	double value;
};

void printQuote(struct Quote* quote);
char* getQuoteQuery(char* series, char* query);
char* getQuoteSymbolRangeQuery(char* symbol, unsigned int start, unsigned int end);



#endif