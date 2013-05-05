#ifndef __CRUNCHSERIES_PROCESS_QUOTE_H__
#define __CRUNCHSERIES_PROCESS_QUOTE_H__

#include <mysql.h>

struct _QUOTES {
	unsigned int count;
	unsigned int* utime;
	double* high;
	double* low;
	double* open;
	double* close;
	unsigned int* volume;
} typedef QUOTES;

QUOTES* getQuotes(MYSQL_RES* res);
void printQuote(QUOTES* quotes, int place);

char* getQuoteQS(char* series, char* query);
char* getQuoteQSRange(char* symbol, unsigned int begin, unsigned int end);

#endif