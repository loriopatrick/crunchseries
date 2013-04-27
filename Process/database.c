#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <mysql.h>

#include "quote.h"
#include "database.h"

MYSQL* conn = 0;

pthread_mutex_t queryDBLock;
DB_RES* DB_query(char* query) {
	if (!conn) return 0;
	pthread_mutex_lock(&queryDBLock);
		mysql_query(conn, query);
		DB_RES* res = mysql_store_result(conn);
	pthread_mutex_unlock(&queryDBLock);
	return res;
}

void initDBSync() {
	pthread_mutex_init(&queryDBLock, 0);
}

int getQuote(DB_RES* res, QUOTE* quote) {
	MYSQL_ROW row;
	row = mysql_fetch_row(res);
	if (!row) return 0;
	memcpy(quote->symbol, row[0], 8);
	quote->utime = atol(row[1]);
	quote->high = atof(row[2]);
	quote->low = atof(row[3]);
	quote->open = atof(row[4]);
	quote->close = atof(row[5]);
	quote->volume = atoi(row[6]);
	return 1;
}

int DB_numRows(DB_RES* res) {
	return mysql_num_rows(res);
}

void printDBErrors() {
	printf("MYSQL ERROR: %s\n", mysql_error(conn));
}

void DB_connect() {
	if (conn) return;
	initDBSync();
	mysql_library_init(0, 0, 0);
	conn = mysql_init(0);
	mysql_real_connect(conn, "localhost", "root", "root", "crunchseries", 0, 0, 0);
}

void DB_close() {
	if (!conn) return;
	mysql_close(conn);
	conn = 0;
}

void DB_freeRes(DB_RES* res) {
	mysql_free_result(res);
}