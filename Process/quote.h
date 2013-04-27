#ifndef __CRUNCHSERIES_PROCESS_QUOTE_H__
#define __CRUNCHSERIES_PROCESS_QUOTE_H__

#include <mysql.h>

typedef MYSQL_RES DB_RES;

struct _QUOTE {
	char symbol[8];
	unsigned int utime;
	double high;
	double low;
	double open;
	double close;
	unsigned int volume;
} typedef QUOTE;

struct _TIMEVALUE {
	unsigned int utime;
	double value;
} typedef TIMEVALUE;

int QUOTE_get(DB_RES* res, QUOTE* quote);
void QUOTE_print(QUOTE* quote);
char* QUOTE_getQS(char* series, char* query);
char* QUOTE_getQSRange(char* symbol, unsigned int begin, unsigned int end);

#endif