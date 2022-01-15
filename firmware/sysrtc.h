#ifndef __STARPOINTER_SYS_RTC_HEADER__
#define __STARPOINTER_SYS_RTC_HEADER__

#include "common.h"

void initRTC();

void setSystemDateTime(struct tm *timeInfo);
void getSystemDateTime(struct tm *timeInfo);

#endif /* __STARPOINTER_SYS_RTC_HEADER__ */