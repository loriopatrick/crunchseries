#ifndef __CALC_H__
#define __CALC_H__

#include "database.h"
#include "quote.h"

struct _CalcStream {
	int len;
	int buffer;
	void (**stats)(struct TimePair* result, struct Quote* quote, void* memory);
	void** memories;
	struct TimePair** results;
} typedef CalcStream;

void initCalcStream(CalcStream* calc);
void addCalcStreamStat(CalcStream* calc, void (*stat)(struct TimePair* result, struct Quote* quote, void* memory), void* memory);
void executeCalcStream(CalcStream* calc, DBRes* res, int quotes);
int doCalcStream(CalcStream* calc, char* series, char* query);
void freeCalcStream(CalcStream* calc);
#endif