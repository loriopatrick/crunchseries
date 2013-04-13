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

int getQuote(MYSQL_RES* mysql_res, struct Quote* quote);
void printQuote(struct Quote* quote);
char* getQuoteQuery(char* series, char* query);
char* getQuoteQueryBySymbol(char* series, char* symbol, char* foo);

#endif