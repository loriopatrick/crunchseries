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

double* slope(double* values, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 1; i < len; ++i) {
		results[i] = values[i] - values[i - 1];
	}
	results[0] = results[1];

	return results;
}

double* percentDifference(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		if (a[i] + b[i] == 0) {
			results[i] = 0;
			continue;
		}
		results[i] = (a[i] - b[i]) * 200.0 / (a[i] + b[i]);
	}

	return results;
}

double* trendLine(double* values, int len, int period_size) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i];
	}

	return results;
}

double* longYield(double* series, double* buy, int len) {
	double* results = malloc(sizeof(double) * len);
	double value = 1;

	int i;
	for (i = 0; i < len; ++i) {
		if (i > 0 && buy[i - 1] > 0) {
			value *= series[i] / series[i - 1];
		}

		results[i] = value;
	}

	return results;
}

double* sinceHighInPeriod(double* values, int len, int period_size) {
	double* results = malloc(sizeof(double) * len);
	if (!len) {
		return results;
	}
	
	int periods_since = 0;
	results[0] = 0;
	double high = values[0];

	int i, j;
	for (i = 1; i < len; ++i) {
		++periods_since;
		if (values[i] >= high) {
			high = values[i];
			periods_since = 0;
		}

		if (periods_since >= period_size) {
			high = values[i - period_size + 1];
			periods_since = period_size - 1;

			for (j = i - period_size + 2; j <= i; ++j) {
				if (values[j] >= high) {
					high = values[j];
					periods_since = i - j;
				}
			}
		}

		results[i] = (double)periods_since;
	}

	return results;
}

double* sinceLowInPeriod(double* values, int len, int period_size) {
	double* results = malloc(sizeof(double) * len);
	if (!len) return results;
	
	int periods_since = 0;
	results[0] = 0;
	double low = values[0];

	int i, j;
	for (i = 1; i < len; ++i) {
		++periods_since;
		if (values[i] <= low) {
			periods_since = 0;
			low = values[i];
		}

		if (periods_since >= period_size) {
			low = values[i - period_size + 1];
			periods_since = period_size - 1;

			for (j = i - period_size + 2; j <= i; ++j) {
				if (values[j] <= low) {
					low = values[j];
					periods_since = i - j;
				}
			}
		}

		results[i] = (double)periods_since;
	}

	return results;
}