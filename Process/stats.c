#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stats.h"

struct stream_tail {
	int size;
	double *values;
	int pos;
};

void initTail(struct stream_tail* tail, int size) {
	tail->size = size;
	tail->pos = 0;
	int bytes = sizeof(double) * tail->size;
	tail->values = malloc(bytes);
	memset(tail->values, 0, bytes);
}

void updateTail(struct stream_tail* tail, double value) {
	tail->values[tail->pos++] = value;
	if (tail->pos == tail->size) {
		tail->pos = 0;
	}
}

double* simpleMovingAverage(double* values, int len, int period_size) {
	double* results = malloc(sizeof(double) * len);

	struct stream_tail tail;
	initTail(&tail, period_size);

	double lastAverage = 0;

	int i;
	for (i = 0; i < len; ++i) {
		double pop = tail.values? tail.values[tail.pos] : 0;
		updateTail(&tail, values[i]);
		results[i] = lastAverage + (values[i] - pop) / period_size;
		lastAverage = results[i];
	}

	return results;
}

double* exponentialMovingAverage(double* values, int len, int period_size) {
	double* results = malloc(sizeof(double) * len);
	double k = 2.0 / (period_size + 1.0);
	
	double lastAverage = 0;

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] * k + lastAverage * (1 - k);
		lastAverage = results[i];
	}

	return results;
}

double* standardDeviation(double* values, int len, int period_size) {
	double* results = malloc(sizeof(double) * len);

	struct stream_tail tail;
	initTail(&tail, period_size);

	double s1 = 0, s2 = 0;

	int i;
	for (i = 0; i < len; ++i) {
		s1 += values[i] - tail.values[tail.pos];
		s2 += pow(values[i], 2) - pow(tail.values[tail.pos], 2);

		updateTail(&tail, values[i]);

		results[i] = sqrt((double)period_size * s2 - pow(s1, 2)) / period_size;
	}

	return results;
}