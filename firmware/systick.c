/**********************************************************************************
  StarPointer - Firmware for STM32F103C8.
  SysTick driver.

  Copyright (c) 2022 Dilshan R Jayakody.
   
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
**********************************************************************************/

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