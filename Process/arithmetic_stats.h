#ifndef __CRUNCHSERIES_PROCESS_ARITHMETIC_STATS_H__
#define __CRUNCHSERIES_PROCESS_ARITHMETIC_STATS_H__

double* addNumber(double* values, int len, double number);
double* subtractNumber(double* values, int len, double number);
double* multiplyNumber(double* values, int len, double number);
double* powerNumber(double* values, int len, double number);
double* exponentiateNumber(double* values, int len, double number);

double* sum(double* a, double* b, int len);
double* difference(double* a, double* b, int len);
double* product(double* a, double* b, int len);
double* quotient(double* a, double* b, int len);
double* power(double* a, double* b, int len);

#endif