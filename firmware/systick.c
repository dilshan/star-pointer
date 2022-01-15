#include <libopencm3/cm3/systick.h>

#ifndef SYS_CPU_FREQ
#error "System CPU frequency value (SYS_CPU_FREQ) is undefined"
#endif

volatile uint32_t sysTickMs;

void sys_tick_handler(void)
{
    // SysTick ISR.
    sysTickMs++;
}

void initSysTick()
{
    sysTickMs = 0;
    
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);

    // Configure overflow interrupt in every 1ms.
    systick_set_reload(SYS_CPU_FREQ/1000);

    systick_interrupt_enable();
    systick_counter_enable();
}

void systickDelay(uint32_t delayMs)
{
    uint32_t currentTickValue = sysTickMs;
    while((sysTickMs - currentTickValue) < delayMs);
}