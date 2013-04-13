#include <stdlib.h>
#include <stdio.h>

#include "quote.h"

#include "stats.h"


/*
	Accumulation Distribution Line
*/
double money_flow_multiplier(struct Quote* quote) {
	double demon = quote->high - quote->low;
	if (demon == 0) {
		return 0.0;
	}
	return ((quote->close - quote->low) - (quote->high - quote->close)) / demon;
}

double money_flow_volume(struct Quote* quote) {
	return money_flow_multiplier(quote) * quote->volume;
}

void accumulationDistribution(double* result, struct Quote* quote, void* state) {
	double* last = (double*)state;
	*result = *last + money_flow_volume(quote);
	(*last) = (*result);
}


/*
	Arron up/down
*/
void aroonUp(double* result, struct Quote* quote, void* state) {
	struct aroon* info = (struct aroon*) state;
	
}

void aroonDown(double* result, struct Quote* quote, void* state) {
}