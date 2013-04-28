#ifndef __CRUNCHSERIES_PROCESS_QSTREAMCALC_H__
#define __CRUNCHSERIES_PROCESS_QSTREAMCALC_H__

#include "database.h"
#include "quote.h"

struct _QSTREAM_CALC {
	int len;
	int buffer;
	void (**stats)(TIMEVALUE*, QUOTE*, void*);
	void (**free_mems)(void*);
	void** memories;
	TIMEVALUE** results;
} typedef QSTREAM_CALC;

void QSTREAM_CALC_init(QSTREAM_CALC* calc);
void QSTREAM_CALC_addStat(QSTREAM_CALC* calc, void (*stat)(TIMEVALUE*, QUOTE*, void*), void* mem, void(*free_mem)(void*));
void QSTREAM_CALC_execute(QSTREAM_CALC* calc, DB_RES* res, int quotes);
int QSTREAM_CALC_do(QSTREAM_CALC* calc, char* series, char* query);
void QSTREAM_CALC_free(QSTREAM_CALC* calc);

#endif