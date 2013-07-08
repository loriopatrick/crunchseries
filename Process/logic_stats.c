#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logic_stats.h"

double* aboveThresholdNumber(double* values, int len, double bottom) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] > bottom? 1.0 : 0.0;
	}

	return results;
}

double* bellowThresholdNumber(double* values, int len, double top) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] < top? 1.0 : 0.0;
	}

	return results;
}

double* betweenThresholdNumber(double* values, int len, double bottom, double top) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] > bottom && values[i] < top? 1.0 : 0.0;
	}

	return results;
}

double* aboveThreshold(double* values, double* threshold, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] > threshold[i]? 1.0 : 0.0;
	}

	return results;
}

double* bellowThreshold(double* values, double* threshold, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] < threshold[i]? 1.0 : 0.0;
	}

	return results;
}

double* betweenThreshold(double* values, double* bottom, double* top, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] > bottom[i] && values[i] < top[i]? 1.0 : 0.0;
	}

	return results;
}

double* andGate(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = a[i] > 0 && b[i] > 0? 1.0 : 0.0;
	}

	return results;
}

double* orGate(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = a[i] > 0 || b[i] > 0? 1.0 : 0.0;
	}

	return results;
}

double* xorGate(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = (a[i] > 0 && b[i] == 0) || (a[i] == 0 && b[i] > 0)? 1.0 : 0.0;
	}

	return results;
}

double* offsetSeries(double* values, int len, int steps) {
	int new_len = len + steps;
	double* results = malloc(sizeof(double) * new_len);

	int i;
	for (i = 0; i < new_len; ++i) {
		if (i < steps || i > len) {
			results[i] = 0;
			continue;
		}

		results[i] = values[i - steps];
	}

	return results;
}