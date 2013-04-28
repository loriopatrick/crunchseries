#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "streamStats.h"

void STREAM_TAIL_update(STREAM_TAIL* tail, double* value) {
	if (!tail->values) {
		int size = sizeof(double) * tail->size;
		tail->values = malloc(size);
		memset(tail->values, 0, size);
	}
	tail->values[tail->pos++] = *value;
	if (tail->pos == tail->size) {
		tail->pos = 0;
	}
}

void STREAM_TAIL_free(STREAM_TAIL* tail) {
	free(tail->values);
}

struct movingAverage {
	double average;
	STREAM_TAIL tail;
};

void* STREAM_STAT_movingAverage_mem(int tail_size) {
	struct movingAverage* mem = malloc(sizeof(struct movingAverage));
	memset(mem, 0, sizeof(struct movingAverage));
	mem->tail.size = tail_size;
	return mem;
}

void STREAM_STAT_movingAverage_mem_free(void* mem) {
	struct movingAverage* ma = (struct movingAverage*)mem;
	STREAM_TAIL_free(&ma->tail);
}

void STREAM_STAT_movingAverageSimple(double* result, double* value, void* mem) {
	struct movingAverage* ma = (struct movingAverage*) mem;
	double pop = ma->tail.values? ma->tail.values[ma->tail.pos] : 0;
	STREAM_TAIL_update(&ma->tail, value);
	ma->average += (*value - pop) / ma->tail.size;
	*result = ma->average;
}

struct movingAverageE {
	double last;
	int quotes;
};

void* STREAM_STAT_movingAverageExponetial_mem(int tail_size) {
	struct movingAverageE* mem = malloc(sizeof(struct movingAverageE));
	mem->last = 0;
	mem->quotes = tail_size;
	return mem;
}

void STREAM_STAT_movingAverageExponetial(double* result, double* value, void* mem) {
	struct movingAverageE* ema = (struct movingAverageE*) mem;
	double k = 2 / (ema->quotes + 1);
	*result = *value * k + ema->last * (1 - k);
	ema->last = *result;
}

struct standardDeviation {
	double s1;
	double s2;
	double value;
	STREAM_TAIL tail;
};

void* STREAM_STAT_standardDeviation_mem(int tail_size) {
	struct standardDeviation* mem = malloc(sizeof(struct standardDeviation));
	memset(mem, 0, sizeof(struct standardDeviation));
	mem->tail.size = tail_size;
	return mem;
}

void STREAM_STAT_standardDeviation_mem_free(void* mem) {
	struct standardDeviation* stdv = (struct standardDeviation*)mem;
	STREAM_TAIL_free(&stdv->tail);
}

void STREAM_STAT_standardDeviation(double* result, double* value, void* mem) {
	struct standardDeviation* std = (struct standardDeviation*) mem;
	double pop = std->tail.values? std->tail.values[std->tail.pos] : 0;

	std->s1 += *value - pop;
	std->s2 += pow(*value, 2) - pow(pop, 2);

	STREAM_TAIL_update(&std->tail, value);

	int n = std->tail.size;
	*result = sqrt((double)n * std->s2 - pow(std->s1, 2)) / n;
	std->value = *result;
}

struct percentB {
	struct movingAverage ma;
	struct standardDeviation stdv;
};

void* STREAM_STAT_percentB_mem(int tail_size) {
	struct percentB* mem = malloc(sizeof(struct percentB));
	memset(mem, 0, sizeof(struct percentB));
	mem->ma.tail.size = tail_size;
	mem->stdv.tail.size = tail_size;
	return mem;
}

void STREAM_STAT_percentB_mem_free(void* mem) {
	struct percentB* pb = (struct percentB*)mem;
	STREAM_STAT_movingAverage_mem_free(&pb->ma);
	STREAM_STAT_standardDeviation_mem_free(&pb->stdv);
}

void STREAM_STAT_percentB(double* result, double* value, void* mem) {
	struct percentB* pb = (struct percentB*)mem;
	double ma, stdv;
	STREAM_STAT_movingAverageSimple(&ma, value, &pb->ma);
	STREAM_STAT_standardDeviation(&stdv, value, &pb->stdv);
	*result = (*value - ma - stdv) / (stdv * 2);
}

struct macd {
	struct movingAverageE small;
	struct movingAverageE big;
};

void* STREAM_STAT_macd_mem(int tail_size_small, int tail_size_big) {
	struct macd* mem = malloc(sizeof(struct macd));
	macd->small.last = 0;
	macd->small.quotes = tail_size_small;
	macd->big.last = 0;
	macd->big.quotes = tail_size_big;
	return mem;
}

void STREAM_STAT_macd(double* result, double* value, void* mem) {
	double small, big;
	struct macd* macd = (struct macd*)mem;

	STREAM_STAT_movingAverageExponetial(&small, value, macd);
	STREAM_STAT_movingAverageExponetial(&big, value, macd);

	*result = small - big;
}

struct macd_signal {
	struct macd macd;
	struct movingAverageE ema;
};

void* STREAM_STAT_macd_signal_mem(int small, int big, int ema) {
	struct macd_signal* macds = malloc(sizeof(struct macd_signal));
	macds->macd.small.last = 0;
	macds->macd.small.quotes = small;
	macds->macd.big.last = 0;
	macds->macd.big.quotes = big;
	macds->ema.quotes = ema;
	macds->ema.last = 0;
}

void STREAM_STAT_macd_signal(double* result, double* value, void* mem) {
	struct macd_signal* macds = (struct macd_signal*)mem;
	double macd;

	STREAM_STAT_macd(&macd, value, macds->macd);
	STREAM_STAT_movingAverageExponetial(result, &macd, mem->ema);
}