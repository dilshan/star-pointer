#ifndef __STARPOINTER_DATE_TIME_HEADER__
#define __STARPOINTER_DATE_TIME_HEADER__

#include "common.h"

#define YEAR_BASE	1900

time_t mktime(struct tm *tim_p);
struct tm *gmtime_r(const time_t *tim_p, struct tm *res);

#endif /* __STARPOINTER_DATE_TIME_HEADER__ */