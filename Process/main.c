#include <stdio.h>
#include <stdlib.h>

#include <mysql.h>

#include "quote.h"

int main (int args, char** argv) {
	char* test = getQuoteQuery("eom", "WHERE epoch>12");
	printf("%s\n", test);
	return 0;
}