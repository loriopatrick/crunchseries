#ifndef __STATS_H__
#define __STATS_H__

/*
	reference: http://stockcharts.com/school/doku.php?id=chart_school:technical_indicators
*/

#include "quote.h"

void* STREAMSTAT_accumulationDistribution_mem(double start);
void STREAMSTAT_accumulationDistribution(TimeValue* result, struct Quote* quote, void* mem);

void* STREAMSTAT_aroon_mem(int tail_size);
void STREAMSTAT_aroonUp(TimeValue* result, Quote* quote, void* mem);
void STREAMSTAT_aroonDown(TimeValue* result, Quote* quote, void* mem);

void* STREAMSTAT_movingAverage_mem(int tail_size);
void STREAMSTAT_movingAverageSimple(TimeValue* result, Quote* quote, void* mem);

void* STREAMSTAT_standardDeviation_mem(int tail_size);
void STREAMSTAT_standardDeviation(TimeValue* result, Quote* quote, void* mem);

#endif