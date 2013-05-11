#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <mysql.h>

#include "database.h"

MYSQL* conn = 0;

pthread_mutex_t queryDBLock;
MYSQL_RES* DB_query(char* query) {
	if (!conn) return 0;
	pthread_mutex_lock(&queryDBLock);
		mysql_query(conn, query);
		MYSQL_RES* res = mysql_store_result(conn);
	pthread_mutex_unlock(&queryDBLock);
	return res;
}

void initDBSync() {
	pthread_mutex_init(&queryDBLock, 0);
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