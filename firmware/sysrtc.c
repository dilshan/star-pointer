#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rtc.h>

#include "common.h"
#include "datetime.h"

#define DEFAULT_TIME_STAMP  1642259203

void initRTC()
{
    rcc_enable_rtc_clock();

    // Disable all RTC related interrupts.
    rtc_interrupt_disable(RTC_SEC);
    rtc_interrupt_disable(RTC_ALR);
    rtc_interrupt_disable(RTC_OW);

    // Configure RTC to use LSE oscillator.
    rtc_awake_from_off(RCC_LSE);
    rtc_set_prescale_val(0x7FFF);
    
    rtc_set_counter_val(DEFAULT_TIME_STAMP);

    // Enable RTC interrupts.
    nvic_enable_irq(NVIC_RTC_IRQ);

    rtc_clear_flag(RTC_SEC);
    rtc_interrupt_enable(RTC_SEC);
}

void setSystemDateTime(struct tm *timeInfo)
{
    time_t timeStamp;

    timeStamp = mktime(timeInfo);
    rtc_set_counter_val(timeStamp);
}

void getSystemDateTime(struct tm *timeInfo)
{
    time_t timeStamp;

    timeStamp = rtc_get_counter_val();
    gmtime_r(&timeStamp, timeInfo);
}

void rtc_isr(void)
{
    // RTC seconds interrupt.
    if(rtc_check_flag(RTC_SEC))
    {
        rtc_clear_flag(RTC_SEC);
    }
}