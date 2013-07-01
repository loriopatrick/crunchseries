#ifndef __CRUNCHSERIES_LOGIC_STATS_H__
#define __CRUNCHSERIES_LOGIC_STATS_H__

double* aboveThreshold(double* values, int len, double bottom);
double* bellowThreshold(double* values, int len, double top);
double* betweenThreshold(double* values, int len, double bottom, double top);
double* andGate(double* a, double* b, int len);
double* orGate(double* a, double* b, int len);
double* xorGate(double* a, double* b, int len);
double* offsetSeries(double* values, int len, int steps);

#endif