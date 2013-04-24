#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "quote.h"
#include "qStreamStats.h"

struct tail {
	int size;
	double *values;
	int pos;
};

void updateTail(Quote* quote, struct tail* tail) {
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

double moneyFlowMultiplier(Quote* quote) {
	double denom = quote->high - quote->low;
	if (denom == 0) {
		return 0.0;
	}
	return ((quote->close - quote->low) - (quote->high - quote->close)) / denom;
}

double moneyFlowVolume(Quote* quote) {
	return moneyFlowMultiplier(quote) * quote->volume;
}

void* QSTREAMSTAT_accumulationDistribution_mem(double start) {
	double* mem = malloc(sizeof(double));
	*mem = start;
	return mem;
}

void QSTREAMSTAT_accumulationDistribution(TimeValue* result, Quote* quote, void* mem) {
	double* last = (double*)mem;
	result->time = quote->time;
	result->value = *last + moneyFlowVolume(quote);
	(*last) = (result->value);
}

struct aroon {
	struct tail tail;

	double high;
	int quotes_since_high;
	
	double low;
	int quotes_since_low;
};

void* QSTREAMSTAT_aroon_mem(int tail_size) {
	struct aroon* mem = malloc(sizeof(struct aroon));
	memset(mem, 0, sizeof(struct aroon));
	mem->tail.size = tail_size;
	return mem;
}

void QSTREAMSTAT_aroonUp(TimeValue* result, Quote* quote, void* mem) {
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

	result->time = quote->time;
	result->value = ((double)(aroon->tail.tail_size - aroon->days_since_high) / (double)aroon->tail.tail_size) * 100.0;
}

void QSTREAMSTAT_aroonDown(TimeValue* result, Quote* quote, void* mem) {
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

	result->time = quote->time;
	result->value = ((double)(aroon->tail.tail_size - aroon->days_since_high) / (double)aroon->tail.tail_size) * 100;
}

struct movingAverage {
	double average;
	struct tail tail;
};

void* QSTREAMSTAT_movingAverage_mem(int tail_size) {
	struct movingAverage* mem = malloc(sizeof(struct movingAverage));
	memset(mem, 0, sizeof(struct movingAverage));
	mem->tail.size = tail_size;
	return mem;
}

void QSTREAMSTAT_simpleMovingAverage(TimeValue* result, Quote* quote, void* mem) {
	struct movingAverage* ma = (struct movingAverage*) mem;
	double pop = ma->tail.values? ma->tail.values[ma->tail.pos] : 0;
	updateTail(quote, &ma->tail);
	ma->average += (quote->close - pop) / ma->tail.tail_size;
	result->time = quote->time;
	result->value = ma->average;
}

struct standardDeviation {
	double s1;
	double s2;
	struct tail tail;
};

void* QSTREAMSTAT_standardDeviation_mem(int tail_size) {
	struct standardDeviation* mem = malloc(sizeof(struct standardDeviation));
	memset(mem, 0, sizeof(struct standardDeviation));
	mem->tail.size = tail_size;
	return mem;
}

void QSTREAMSTAT_standardDeviation(TimeValue* result, Quote* quote, void* mem) {
	struct standardDeviation* std = (struct standardDeviation*) mem;
	double pop = std->tail.values? std->tail.values[std->tail.pos] : 0;

	std->s1 += quote->close - pop;
	std->s2 += pow(quote->close, 2) - pow(pop, 2);

	updateTail(quote, &std->tail);

	result->time = quote->time;
	int n = std->tail.tail_size;
	result->value = sqrt((double)n * std->s2 - pow(std->s1, 2)) / n;
}