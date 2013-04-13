#ifndef __CALC_H__
#define __CALC_H__

#include <mysql.h>
#include "quote.h"

void calc(MYSQL_RES* mysql_res, int quotes, int stats, double** result, void (*calc_stats)(double* result, struct Quote* quote, void* state), void** states);

#endif