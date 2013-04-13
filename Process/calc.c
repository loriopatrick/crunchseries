#include <stdio.h>
#include <stdlib.h>

#include <mysql.h>

#include "quote.h"

void calc(MYSQL_RES* mysql_res, int quotes, int stats, double** results, void (*calc_stats)(double* result, struct Quote* quote, void* state), void** states) {

	struct Quote quote;
	
	int i, j;
	for (i = 0; i < quotes; ++i) {
		if (!getQuote(mysql_res, &quote)) break;
		for (j = 0; j < stats; ++j) {
			// printf("%p :: %f\n", states[j], *((double*)states[j]));
			(calc_stats + j)(results[j] + i, &quote, states[j]);
		}
	}
}