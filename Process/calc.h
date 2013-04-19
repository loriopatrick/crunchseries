#ifndef __CALC_H__
#define __CALC_H__

#include <mysql.h>
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
void executeCalc(MYSQL_RES* mysql_res, int quotes, struct Calc* calc, MYSQL* conn);
void doCalc(char* series, char* query, struct Calc* calc, MYSQL* conn);
void freeCalc(struct Calc* calc);
#endif