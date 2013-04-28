#ifndef __CRUNCHSERIES_PROCESS_QSTREAMSTATS_H__
#define __CRUNCHSERIES_PROCESS_QSTREAMSTATS_H__

/*
	reference: http://stockcharts.com/school/doku.php?id=chart_school:technical_indicators
	for stat math
*/

#include "quote.h"

void* QSTREAM_STAT_accumulationDistribution_mem(double start);
void QSTREAM_STAT_accumulationDistribution(TIMEVALUE* result, QUOTE* quote, void* mem);

void* QSTREAM_STAT_aroon_mem(int tail_size);
void QSTREAM_STAT_aroonUp(TIMEVALUE* result, QUOTE* quote, void* mem);
void QSTREAM_STAT_aroonDown(TIMEVALUE* result, QUOTE* quote, void* mem);

void* QSTREAM_STAT_movingAverage_mem(int tail_size);
void QSTREAM_STAT_movingAverageSimple(TIMEVALUE* result, QUOTE* quote, void* mem);

void* QSTREAM_STAT_standardDeviation_mem(int tail_size);
void QSTREAM_STAT_standardDeviation(TIMEVALUE* result, QUOTE* quote, void* mem);

void* QSTREAM_STAT_percentB_mem(int tail_size);
void QSTREAM_STAT_percentB(TIMEVALUE* result, QUOTE* quote, void* mem);

#endif