#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "arithmetic_stats.h"

double* addNumber(double* values, int len, double number) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] + number;
	}

	return results;
}

double* multiplyNumber(double* values, int len, double number) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = values[i] * number;
	}

	return results;
}

double* powerNumber(double* values, int len, double number) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = pow(results[i], number);
	}

	return results;
}

double* exponentiateNumber(double* values, int len, double number) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = pow(number, results[i]);
	}

	return results;
}

double* sum(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = a[i] + b[i];
	}

	return results;
}

double* difference(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = a[i] - b[i];
	}

	return results;
}

double* product(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = a[i] * b[i];
	}

	return results;
}

double* quotient(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		if (b[i] == 0) {
			results[i] = 0;
			continue;
		}
		results[i] = a[i] * 1.00 / b[i];
	}

	return results;
}

double* power(double* a, double* b, int len) {
	double* results = malloc(sizeof(double) * len);

	int i;
	for (i = 0; i < len; ++i) {
		results[i] = pow(a[i], b[i]);
	}

	return results;
}