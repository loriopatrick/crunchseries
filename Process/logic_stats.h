#ifndef __CRUNCHSERIES_LOGIC_STATS_H__
#define __CRUNCHSERIES_LOGIC_STATS_H__

double* aboveThresholdNumber(double* values, int len, double bottom);
double* bellowThresholdNumber(double* values, int len, double top);
double* betweenThresholdNumber(double* values, int len, double bottom, double top);
double* aboveThreshold(double* values, double* threshold, int len);
double* bellowThreshold(double* values, double* threshold, int len);
double* betweenThreshold(double* values, double* bottom, double* top, int len);
double* andGate(double* a, double* b, int len);
double* orGate(double* a, double* b, int len);
double* xorGate(double* a, double* b, int len);
double* offsetSeries(double* values, int len, int steps);

#endif