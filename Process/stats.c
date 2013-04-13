#include <stdlib.h>
#include <stdio.h>

#include "quote.h"

#include "stats.h"


void updateTail(struct Quote* quote, struct tail* tail) {
	if (!tail->values) tail->values = malloc(sizeof(double) * tail->tail_size);
	tail->values[tail->pos++] = quote->close;
	if (tail->pos == tail->tail_size) {
		tail->pos = 0;
	}
}

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
	Aroon up/down

	Aroon-Up = ((25 - Days Since 25-day High)/25) x 100
	Aroon-Down = ((25 - Days Since 25-day Low)/25) x 100
*/
void aroonUp(double* result, struct Quote* quote, void* state) {
	struct aroon* aroon = (struct aroon*) state;

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

	*result = ((double)(aroon->tail.tail_size - aroon->days_since_high) / (double)aroon->tail.tail_size) * 100.0;
}

void aroonDown(double* result, struct Quote* quote, void* state) {
	struct aroon* aroon = (struct aroon*) state;

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

	*result = ((double)(aroon->tail.tail_size - aroon->days_since_high) / (double)aroon->tail.tail_size) * 100;
}