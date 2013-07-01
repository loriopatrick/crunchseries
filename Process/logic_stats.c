#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logic_stats.h"

double* aboveThreshold(double* values, int len, double bottom) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] > bottom? 1.0 : 0.0;
	}

	return results;
}

double* bellowThreshold(double* values, int len, double top) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] < top? 1.0 : 0.0;
	}

	return results;
}

double* betweenThreshold(double* values, int len, double bottom, double top) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] < top && values[i] > bottom? 1.0 : 0.0;
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