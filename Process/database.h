#ifndef __CRUNCHSERIES_PROCESS_DATABASE_H__
#define __CRUNCHSERIES_PROCESS_DATABASE_H__

#include <mysql.h>
#include "quote.h"

typedef MYSQL_RES DB_RES;
typedef MYSQL_ROW DB_ROW;

DB_RES* DB_query(char* query);
int DB_numRows(DB_RES* res);

void DB_connect();
void DB_close();
void DB_freeRes(DB_RES *res);

#endif