#ifndef __CALC_H__
#define __CALC_H__

#include <mysql.h>
#include "quote.h"

struct calc {
	int len;
	int buffer;
	void (**stats)(struct TimePair* result, struct Quote* quote, void* memory);
	void** memories;
	struct TimePair** results;
};


void initCalc(struct calc* calc);
void addCalcStat(struct calc* calc, void (*stat)(struct TimePair* result, struct Quote* quote, void* memory), void* memory);
void executeCalc(MYSQL_RES* mysql_res, int quotes, struct calc* calc);
void doCalc(char* series, char* query, struct calc* calc);
void freeCalc(struct calc* calc);
#endif