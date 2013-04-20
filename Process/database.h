#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <mysql.h>
#include "quote.h"

typedef MYSQL_RES DBRes;

DBRes* queryDB(char* query);
void initDBSync();
int getQuote(DBRes* res, struct Quote* quote);
int getDBResRows(DBRes* res);
void printDBErrors();
void connectDB();
void closeDB();
void freeDBRes(DBRes* res);

#endif