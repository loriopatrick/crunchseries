#ifndef __CRUNCHSERIES_PROCESS_STAT_H__
#define __CRUNCHSERIES_PROCESS_STAT_H__

double* simpleMovingAverage(double* values, int len, int period_size);
double* exponentialMovingAverage(double* values, int len, int period_size);
double* standardDeviation(double* values, int len, int period_size);

double* difference(double* a, double* b, int len);
double* percentDifference(double* a, double* b, int len);
double* sum(double* a, double* b, int len);

#endif