#ifndef __CRUNCHSERIES_PROCESS_DATABASE_H__
#define __CRUNCHSERIES_PROCESS_DATABASE_H__

#include <mysql.h>

MYSQL_RES* DB_query(char* query);
int DB_numRows(MYSQL_RES* res);

void DB_connect();
void DB_close();

#endif