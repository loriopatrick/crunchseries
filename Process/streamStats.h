#ifndef __CRUNCHSERIES_PROCESS_DSTREAMSTATS_H__
#define __CRUNCHSERIES_PROCESS_DSTREAMSTATS_H__

#include "quote.h"

struct _STREAM_TAIL {
	int size;
	double *values;
	int pos;
} typedef STREAM_TAIL;

void STREAM_TAIL_update(STREAM_TAIL* tail, double* value);
void STREAM_TAIL_free(STREAM_TAIL* tail);

void* STREAM_STAT_movingAverage_mem(int tail_size);
void STREAM_STAT_movingAverage_mem_free(void* mem);
void STREAM_STAT_movingAverageSimple(double* result, double* value, void* mem);
void STREAM_STAT_movingAverageExponetial(double* result, double* value, void* mem);

void* STREAM_STAT_standardDeviation_mem(int tail_size);
void STREAM_STAT_standardDeviation_mem_free(void* mem);
void STREAM_STAT_standardDeviation(double* result, double* value, void* mem);

void* STREAM_STAT_percentB_mem(int tail_size);
void STREAM_STAT_percentB_mem_free(void* mem);
void STREAM_STAT_percentB(double* result, double* value, void* mem);

void* STREAM_STAT_movingAverageExponetial_mem(int tail_size);
void STREAM_STAT_movingAverageExponetial(double* result, double* value, void* mem);

struct lastThres {
	STREAM_TAIL tail;
	double thres;
	int since;
	int high;
	int returnType; // 1 = thres, 2 = since
};

void* STREAM_STAT_lastThres_mem(int tail_size, int returnType, int high);
void STREAM_STAT_lastThres(double* result, double* value, void* mem);

#endif