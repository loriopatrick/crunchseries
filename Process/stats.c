#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "quote.h"

#include "stats.h"


void updateTail(struct Quote* quote, struct tail* tail) {
	if (!tail->values) {
		int size = sizeof(double) * tail->tail_size;
		tail->values = malloc(size);
		memset(tail->values, 0, size);
	}
	tail->values[tail->pos++] = quote->close;
	if (tail->pos == tail->tail_size) {
		tail->pos = 0;
	}
}

/*
	Accumulation Distribution Line
*/
double money_flow_multiplier(struct Quote* quote) {
	double denom = quote->high - quote->low;
	if (denom == 0) {
		return 0.0;
	}
	return ((quote->close - quote->low) - (quote->high - quote->close)) / denom;
}

double money_flow_volume(struct Quote* quote) {
	return money_flow_multiplier(quote) * quote->volume;
}

void accumulationDistribution(struct TimePair* result, struct Quote* quote, void* mem) {
	double* last = (double*)mem;
	result->epoch = quote->epoch;
	result->value = *last + money_flow_volume(quote);
	(*last) = (result->value);
	// printf("%u :: %f\n", result->epoch, result->value);
}


/*
	Aroon up/down

	Aroon-Up = ((25 - Days Since 25-day High)/25) x 100
	Aroon-Down = ((25 - Days Since 25-day Low)/25) x 100
*/
void aroonUp(struct TimePair* result, struct Quote* quote, void* mem) {
	struct aroon* aroon = (struct aroon*) mem;

	updateTail(quote, &(aroon->tail));

	if (quote->close > aroon->high) {
		aroon->high = quote->high;
		aroon->days_since_high = 0;
	} else if(aroon->days_since_high == aroon->tail.tail_size) {
		
		int i, high_pos = 0;
		double* high_value = aroon->tail.values;
		for (i = 1; i < aroon->tail.tail_size; ++i) {
			if (*high_value <= *(aroon->tail.values + i)) {
				high_value = aroon->tail.values + i;
				high_pos = i;
			}
		}
		
		aroon->high = *high_value;
		int temp = aroon->tail.pos - high_pos;
		if (temp < 0) {
			aroon->days_since_high = temp + aroon->tail.tail_size;
		} else {
			aroon->days_since_high = temp;
		}

	} else {
		++aroon->days_since_high;
	}

	result->epoch = quote->epoch;
	result->value = ((double)(aroon->tail.tail_size - aroon->days_since_high) / (double)aroon->tail.tail_size) * 100.0;
	// printf("AroonUp: %f\n", result->value);
}

void aroonDown(struct TimePair* result, struct Quote* quote, void* mem) {
	struct aroon* aroon = (struct aroon*) mem;

	updateTail(quote, &(aroon->tail));

	if (quote->close < aroon->low) {
		aroon->low = quote->low;
		aroon->days_since_low = 0;
	} else if(aroon->days_since_low == aroon->tail.tail_size) {
		
		int i, low_pos = 0;
		double* low_value = aroon->tail.values;
		for (i = 1; i < aroon->tail.tail_size; ++i) {
			if (*low_value >= *(aroon->tail.values + i)) {
				low_value = aroon->tail.values + i;
				low_pos = i;
			}
		}
		
		aroon->low = *low_value;
		int temp = aroon->tail.pos - low_pos;
		if (temp < 0) {
			aroon->days_since_low = temp + aroon->tail.tail_size;
		} else {
			aroon->days_since_low = temp;
		}

	} else {
		++aroon->days_since_low;
	}

	result->epoch = quote->epoch;
	result->value = ((double)(aroon->tail.tail_size - aroon->days_since_high) / (double)aroon->tail.tail_size) * 100;
}

void simpleMovingAverage(struct TimePair* result, struct Quote* quote, void* mem) {
	struct movingAverage* ma = (struct movingAverage*) mem;
	double pop = ma->tail.values? ma->tail.values[ma->tail.pos] : 0;
	updateTail(quote, &ma->tail);
	ma->average += (quote->close - pop) / ma->tail.tail_size;
	result->epoch = quote->epoch;
	result->value = ma->average;
	// printf("%f\n\n", result->value);
}

void standardDeviation(struct TimePair* result, struct Quote* quote, void* mem) {
	struct standardDeviation* std = (struct standardDeviation*) mem;
	double pop = std->tail.values? std->tail.values[std->tail.pos] : 0;

	std->s1 += quote->close - pop;
	std->s2 += pow(quote->close, 2) - pow(pop, 2);

	updateTail(quote, &std->tail);

	result->epoch = quote->epoch;
	int n = std->tail.tail_size;
	result->value = sqrt((double)n * std->s2 - pow(std->s1, 2)) / n;

	printf("%f\n", result->value);
}