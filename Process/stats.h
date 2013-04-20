#ifndef __STATS_H__
#define __STATS_H__

/*
Implement everything from: http://stockcharts.com/school/doku.php?id=chart_school:technical_indicators
*/

#include "quote.h"

struct tail {
	int tail_size;
	double *values;
	int pos;
};

// Accumulation Distribution Line
// state = double
double money_flow_multiplier(struct Quote* quote);
double money_flow_volume(struct Quote* quote);
void accumulationDistribution(struct TimePair* result, struct Quote* quote, void* state);


// Arron up/down
struct aroon {
	struct tail tail;

	double high;
	int days_since_high; // todo: refactor to quotes_since_high
	
	double low;
	int days_since_low; // todo: refactor to quotes_since_low
};
void aroonUp(struct TimePair* result, struct Quote* quote, void* state);
void aroonDown(struct TimePair* result, struct Quote* quote, void* state);









// struct movingAverage {
// 	int coverage;
// 	int exponential;
// };

// void movingAverage(double* result, struct Quote* quote, void* state);

#endif