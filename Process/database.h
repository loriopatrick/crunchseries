#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <mysql.h>
#include "quote.h"

typedef MYSQL_RES DBRes;

DBRes* queryDB(char* query);
int getQuote(DBRes* res, struct Quote* quote);
int getDBResRows(DBRes* res);
void connectDB();
void closeDB();
void freeDBRes(DBRes* res);

#endif