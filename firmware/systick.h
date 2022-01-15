#ifndef __STARPOINTER_SYS_TICK_HEADER__
#define __STARPOINTER_SYS_TICK_HEADER__

#include <stdint.h>

extern volatile uint32_t sysTickMs;

void initSysTick(void);
void systickDelay(uint32_t delayMs);

#endif /* __STARPOINTER_SYS_TICK_HEADER__ */