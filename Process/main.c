
#include "network.h"
#include "netHandler.h"
#include "database.h"
#include "quote.h"

int main (int args, char** argv) {
	DB_connect();
	NET_startServer(5032, 20, netHandler);
	DB_close();
	return 0;
}