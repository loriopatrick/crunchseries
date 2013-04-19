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

int getQuote(MYSQL_RES* mysql_res, struct Quote* quote);
void printQuote(struct Quote* quote);
char* getQuoteQuery(char* series, char* query);
char* getQuoteQueryBySymbol(char* series, char* symbol, char* foo);
MYSQL* getConn();
MYSQL_RES* requestQuotes(char* query, MYSQL* conn);
void closeConnections();
void printMYSQLError();


#endif