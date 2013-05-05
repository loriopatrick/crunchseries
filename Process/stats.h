#ifndef __CRUNCHSERIES_PROCESS_STAT_H__
#define __CRUNCHSERIES_PROCESS_STAT_H__

double* simpleMovingAverage(double* values, int len, int period_size);
double* exponentialMovingAverage(double* values, int len, int period_size);
double* standardDeviation(double* values, int len, int period_size);

#endif