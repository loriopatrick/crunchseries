#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <mysql.h>
#include "quote.h"

struct DataConn {
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW last_row;
};

void initDataConn(struct DataConn* conn);
int queryDataConn(struct DataConn* conn, char* query);
int retreiveDataConnQuote(struct DataConn* conn, struct Quote* quote);
void printDataConnError(struct DataConn* conn);
void freeDataConnQuery(struct DataConn* conn);
void freeDataConn(struct DataConn* conn);

#endif