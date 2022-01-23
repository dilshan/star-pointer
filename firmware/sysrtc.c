#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/f1/bkp.h>

#include "common.h"
#include "datetime.h"
#include "logger.h"

#define DEFAULT_TIME_STAMP  1642259203
#define RTC_PRESCALER       0x7FFF
#define RTC_BACKUP_FLAG     0x37B

void initRTC()
{   
    rcc_enable_rtc_clock();

    // Disable all RTC related interrupts.
    rtc_interrupt_disable(RTC_SEC);
    rtc_interrupt_disable(RTC_ALR);
    rtc_interrupt_disable(RTC_OW);
 
    if(BKP_DR1 != RTC_BACKUP_FLAG)
    {    
        // Configure RTC to use LSE oscillator.
        rtc_awake_from_off(RCC_LSE);
        rtc_set_prescale_val(RTC_PRESCALER);

        // (Reset and) start RTC from the default time stamp.   
        rtc_set_counter_val(DEFAULT_TIME_STAMP);
        BKP_DR1 = RTC_BACKUP_FLAG;
    }
    else
    {
        // Continue RTC with backup time.
        rtc_auto_awake(RCC_LSE, RTC_PRESCALER);
    }

    // Enable RTC interrupts.
    nvic_enable_irq(NVIC_RTC_IRQ);
        
    rtc_clear_flag(RTC_SEC);
    rtc_interrupt_enable(RTC_SEC);
}

void setSystemDateTime(struct tm *timeInfo)
{
    time_t timeStamp;
    struct tm tempTime;

    // Create new structure which holds year from the epoch.
    tempTime = *timeInfo;
    tempTime.tm_year -= YEAR_BASE;

    timeStamp = mktime(&tempTime);
    rtc_set_counter_val(timeStamp);
}

void getSystemDateTime(struct tm *timeInfo)
{
    time_t timeStamp;

    timeStamp = rtc_get_counter_val();
    gmtime_r(&timeStamp, timeInfo);
    timeInfo->tm_year += YEAR_BASE;
}

void rtc_isr(void)
{
    // RTC seconds interrupt.
    if(rtc_check_flag(RTC_SEC))
    {
        rtc_clear_flag(RTC_SEC);
    }
}