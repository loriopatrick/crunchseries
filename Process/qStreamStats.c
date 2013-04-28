#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "quote.h"
#include "streamStats.h"
#include "qStreamStats.h"

double moneyFlowMultiplier(QUOTE* quote) {
	double denom = quote->high - quote->low;
	if (denom == 0) {
		return 0.0;
	}
	return ((quote->close - quote->low) - (quote->high - quote->close)) / denom;
}

double moneyFlowVolume(QUOTE* quote) {
	return moneyFlowMultiplier(quote) * quote->volume;
}

void* QSTREAM_STAT_accumulationDistribution_mem(double start) {
	double* mem = malloc(sizeof(double));
	*mem = start;
	return mem;
}

void QSTREAM_STAT_accumulationDistribution(TIMEVALUE* result, QUOTE* quote, void* mem) {
	double* last = (double*)mem;
	result->utime = quote->utime;
	result->value = *last + moneyFlowVolume(quote);
	(*last) = (result->value);
}

struct aroon {
	STREAM_TAIL tail;

	double high;
	int quotes_since_high;
	
	double low;
	int quotes_since_low;
};

void* QSTREAM_STAT_aroon_mem(int tail_size) {
	struct aroon* mem = malloc(sizeof(struct aroon));
	memset(mem, 0, sizeof(struct aroon));
	mem->tail.size = tail_size;
	return mem;
}

void QSTREAM_STAT_aroonUp(TIMEVALUE* result, QUOTE* quote, void* mem) {
	struct aroon* aroon = (struct aroon*) mem;

	STREAM_TAIL_update(&(aroon->tail), &quote->close);

	if (quote->close > aroon->high) {
		aroon->high = quote->high;
		aroon->quotes_since_high = 0;
	} else if(aroon->quotes_since_high == aroon->tail.size) {
		
		int i, high_pos = 0;
		double* high_value = aroon->tail.values;
		for (i = 1; i < aroon->tail.size; ++i) {
			if (*high_value <= *(aroon->tail.values + i)) {
				high_value = aroon->tail.values + i;
				high_pos = i;
			}
		}
		
		aroon->high = *high_value;
		int temp = aroon->tail.pos - high_pos;
		if (temp < 0) {
			aroon->quotes_since_high = temp + aroon->tail.size;
		} else {
			aroon->quotes_since_high = temp;
		}

	} else {
		++aroon->quotes_since_high;
	}

	result->utime = quote->utime;
	result->value = ((double)(aroon->tail.size - aroon->quotes_since_high) / (double)aroon->tail.size) * 100.0;
}

void QSTREAM_STAT_aroonDown(TIMEVALUE* result, QUOTE* quote, void* mem) {
	struct aroon* aroon = (struct aroon*) mem;

	STREAM_TAIL_update(&(aroon->tail), &quote->close);

	if (quote->close < aroon->low) {
		aroon->low = quote->low;
		aroon->quotes_since_low = 0;
	} else if(aroon->quotes_since_low == aroon->tail.size) {
		
		int i, low_pos = 0;
		double* low_value = aroon->tail.values;
		for (i = 1; i < aroon->tail.size; ++i) {
			if (*low_value >= *(aroon->tail.values + i)) {
				low_value = aroon->tail.values + i;
				low_pos = i;
			}
		}
		
		aroon->low = *low_value;
		int temp = aroon->tail.pos - low_pos;
		if (temp < 0) {
			aroon->quotes_since_low = temp + aroon->tail.size;
		} else {
			aroon->quotes_since_low = temp;
		}

	} else {
		++aroon->quotes_since_low;
	}

	result->utime = quote->utime;
	result->value = ((double)(aroon->tail.size - aroon->quotes_since_high) / (double)aroon->tail.size) * 100;
}

void* QSTREAM_STAT_movingAverage_mem(int tail_size) {
	return STREAM_STAT_movingAverage_mem(tail_size);
}

void QSTREAM_STAT_movingAverageSimple(TIMEVALUE* result, QUOTE* quote, void* mem) {
	STREAM_STAT_movingAverageSimple(&result->value, &quote->close, mem);
	result->utime = quote->utime;
}

void* QSTREAM_STAT_standardDeviation_mem(int tail_size) {
	return STREAM_STAT_standardDeviation_mem(tail_size);
}

void QSTREAM_STAT_standardDeviation(TIMEVALUE* result, QUOTE* quote, void* mem) {
	STREAM_STAT_standardDeviation(&result->value, &quote->close, mem);
	result->utime = quote->utime;
}

void* QSTREAM_STAT_percentB_mem(int tail_size) {
	return STREAM_STAT_percentB_mem(tail_size);
}

void QSTREAM_STAT_percentB(TIMEVALUE* result, QUOTE* quote, void* mem) {
	STREAM_STAT_percentB(&result->value, &quote->close, mem);
	result->utime = quote->utime;
}

struct movingAverage {
	double average;
	STREAM_TAIL tail;
};

struct commodityChannelIndex {
	struct movingAverage tpMa;
};

// void* QSTREAM_STAT_commodityChannelIndex_mem(int tail_size) {

// }