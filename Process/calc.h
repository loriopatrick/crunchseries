#ifndef __CALC_H__
#define __CALC_H__

#include "database.h"
#include "quote.h"

struct Calc {
	int len;
	int buffer;
	void (**stats)(struct TimePair* result, struct Quote* quote, void* memory);
	void** memories;
	struct TimePair** results;
};

void initCalc(struct Calc* calc);
void addCalcStat(struct Calc* calc, void (*stat)(struct TimePair* result, struct Quote* quote, void* memory), void* memory);
void executeCalc(struct DataConn* conn, int quotes, struct Calc* calc);
void doCalc(struct DataConn* conn, char* series, char* query, struct Calc* calc);
void freeCalc(struct Calc* calc);
#endif