#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>

#include "quote.h"

#include "database.h"

void initDataConn(struct DataConn* conn) {
	mysql_library_init(0, 0, 0);
	conn->conn = mysql_init(0);
	mysql_real_connect(conn->conn, "localhost", "root", "root", "crunchseries", 0, 0, 0);
	conn->res = 0;
}

int queryDataConn(struct DataConn* conn, char* query) {
	if (conn->res) mysql_free_result(conn->res);
	mysql_query(conn->conn, query);
	conn->res = mysql_store_result(conn->conn);
	if (!conn->res) return -1;
	return mysql_num_rows(conn->res);
}

int retreiveDataConnQuote(struct DataConn* conn, struct Quote* quote) {
	MYSQL_ROW row;
	row = mysql_fetch_row(conn->res);
	if (!row) return 1;
	memcpy(quote->symbol, row[0], 8);
	quote->epoch = atol(row[1]);
	quote->high = atof(row[2]);
	quote->low = atof(row[3]);
	quote->open = atof(row[4]);
	quote->close = atof(row[5]);
	quote->volume = atoi(row[6]);
	return 0;
}

void freeDataConnQuery(struct DataConn* conn) {
	if(!conn->res) return;
	mysql_free_result(conn->res);
	conn->res = 0;
}

void freeDataConn(struct DataConn* conn) {
	freeDataConnQuery(conn);
	mysql_close(conn->conn);
}

void printDataConnError(struct DataConn* conn) {
	printf("MYSQL ERROR: %s\n", mysql_error(conn->conn));
}