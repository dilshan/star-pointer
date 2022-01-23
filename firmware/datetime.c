#include "common.h"
#include "datetime.h"
// Original version extracted from Newlib [https://sourceware.org/newlib/].

typedef struct 
{
  int quot;
  int rem; 
} div_t;

#define SEC_IN_MINUTE  60
#define SEC_IN_HOUR    3600
#define SEC_IN_DAY     86400

#define DAYS_PER_WEEK	    7
#define DAYS_PER_YEAR		365
#define DAYS_IN_JANUARY		31
#define DAYS_IN_FEBRUARY	28
#define YEARS_PER_ERA		400

#define MINS_IN_HOUR	60
#define HOURS_IN_DAY	24

#define EPOCH_ADJUSTMENT_DAYS	719468L
#define ADJUSTED_EPOCH_YEAR	0
#define ADJUSTED_EPOCH_WDAY	3

// there are 97 leap years in 400-year periods. ((400 - 97) * 365 + 97 * 366)
#define DAYS_PER_ERA		146097L

// there are 24 leap years in 100-year periods. ((100 - 24) * 365 + 24 * 366)
#define DAYS_PER_CENTURY	36524L

// there is one leap year every 4 years
#define DAYS_PER_4_YEARS	(3 * 365 + 366)

#define _ISLEAP(y) (((y) % 4) == 0 && (((y) % 100) != 0 || (((y)+1900) % 400) == 0))
#define _ISLEAP_MK(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

#define _DAYS_IN_YEAR(year) (_ISLEAP(year) ? 366 : 365)
#define _DAYS_IN_MONTH(x) ((x == 1) ? days_in_feb : DAYS_IN_MONTH[x])

static const uint8_t DAYS_IN_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint16_t _DAYS_BEFORE_MONTH[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

static div_t div (int num, int denom)
{
    div_t r;

	r.quot = num / denom;
	r.rem = num % denom;

	if((num >= 0) && (r.rem < 0)) 
    {
		++r.quot;
		r.rem -= denom;
	}
	else if((num < 0) && (r.rem > 0)) 
    {
		--r.quot;
		r.rem += denom;
	}

	return (r);
}

static void validate_structure(struct tm *tim_p)
{
    div_t res;
    uint16_t days_in_feb = 28;

    // calculate time & date to account for out of range values
    if((tim_p->tm_sec < 0) || (tim_p->tm_sec > 59))
    {
        res = div (tim_p->tm_sec, 60);
        tim_p->tm_min += res.quot;
        if((tim_p->tm_sec = res.rem) < 0)
        {
            tim_p->tm_sec += 60;
            --tim_p->tm_min;
        }
    }

    if((tim_p->tm_min < 0) || (tim_p->tm_min > 59))
    {
        res = div (tim_p->tm_min, 60);
        tim_p->tm_hour += res.quot;
        if((tim_p->tm_min = res.rem) < 0)
        {
            tim_p->tm_min += 60;
            --tim_p->tm_hour;
        }
    }

    if((tim_p->tm_hour < 0) || (tim_p->tm_hour > 23))
    {
        res = div (tim_p->tm_hour, 24);
        tim_p->tm_mday += res.quot;
        if((tim_p->tm_hour = res.rem) < 0)
        {
            tim_p->tm_hour += 24;
            --tim_p->tm_mday;
        }
    }

    if((tim_p->tm_mon < 0) || (tim_p->tm_mon > 11))
    {
        res = div (tim_p->tm_mon, 12);
        tim_p->tm_year += res.quot;
        if((tim_p->tm_mon = res.rem) < 0)
        {
            tim_p->tm_mon += 12;
            --tim_p->tm_year;
        }
    }

    if(_DAYS_IN_YEAR (tim_p->tm_year) == 366)
        days_in_feb = 29;

    if(tim_p->tm_mday <= 0)
    {
        while(tim_p->tm_mday <= 0)
        {
            if(--tim_p->tm_mon == -1)
            {
                tim_p->tm_year--;
                tim_p->tm_mon = 11;
                days_in_feb = ((_DAYS_IN_YEAR (tim_p->tm_year) == 366) ? 29 : 28);
            }
            tim_p->tm_mday += _DAYS_IN_MONTH (tim_p->tm_mon);
        }
    }
    else
    {
        while(tim_p->tm_mday > _DAYS_IN_MONTH (tim_p->tm_mon))
        {
            tim_p->tm_mday -= _DAYS_IN_MONTH (tim_p->tm_mon);
            if(++tim_p->tm_mon == 12)
            {
                tim_p->tm_year++;
                tim_p->tm_mon = 0;
                days_in_feb = ((_DAYS_IN_YEAR (tim_p->tm_year) == 366) ? 29 : 28);
            }
        }
    }
}

time_t mktime(struct tm *tim_p)
{
    time_t tim = 0;
    uint32_t days = 0;
    uint32_t year, isdst = 0;

    // validate structure
    validate_structure (tim_p);

    // compute hours, minutes, seconds
    tim += tim_p->tm_sec + (tim_p->tm_min * SEC_IN_MINUTE) + (tim_p->tm_hour * SEC_IN_HOUR);

    // compute days in year
    days += tim_p->tm_mday - 1;
    days += _DAYS_BEFORE_MONTH[tim_p->tm_mon];

    if((tim_p->tm_mon > 1) && (_DAYS_IN_YEAR(tim_p->tm_year) == 366))
    {
        days++;
    }

    // compute day of the year
    tim_p->tm_yday = days;

    if ((tim_p->tm_year > 10000) || (tim_p->tm_year < -10000))
    {
        return (time_t) -1;
    }

    // compute days in other years
    if ((year = tim_p->tm_year) > 70)
    {
        for (year = 70; year < tim_p->tm_year; year++)
        {
            days += _DAYS_IN_YEAR(year);
        }
    }
    else if (year < 70)
    {
        for (year = 69; year > tim_p->tm_year; year--)
        {
            days -= _DAYS_IN_YEAR(year);
        }

        days -= _DAYS_IN_YEAR(year);
    }

    // compute total seconds
    tim += (time_t)days * SEC_IN_DAY;

    // compute day of the week
    if ((tim_p->tm_wday = (days + 4) % 7) < 0)
    {
        tim_p->tm_wday += 7;
    }

    return tim;
}

struct tm *gmtime_r(const time_t *tim_p, struct tm *res)
{
    uint32_t days, rem;  
    int32_t era, weekday, year;
    uint32_t erayear, yearday, month, day;
    uint32_t eraday;

    const time_t lcltime = *tim_p;

    days = lcltime / SEC_IN_DAY + EPOCH_ADJUSTMENT_DAYS;
    rem = lcltime % SEC_IN_DAY;

    if(rem < 0)
    {
        rem += SEC_IN_DAY;
        --days;
    }

    // compute hour, min, and sec.
    res->tm_hour = (int) (rem / SEC_IN_HOUR);
    rem %= SEC_IN_HOUR;
    res->tm_min = (int) (rem / SEC_IN_MINUTE);
    res->tm_sec = (int) (rem % SEC_IN_MINUTE);

    // compute day of week.
    if((weekday = ((ADJUSTED_EPOCH_WDAY + days) % DAYS_PER_WEEK)) < 0)
    {
        weekday += DAYS_PER_WEEK;
    }

    res->tm_wday = weekday;

    // compute year, month, day & day of year
    // for description of this algorithm see
    // http://howardhinnant.github.io/date_algorithms.html#civil_from_days
    era = (days >= 0 ? days : days - (DAYS_PER_ERA - 1)) / DAYS_PER_ERA;
    eraday = days - era * DAYS_PER_ERA;	/* [0, 146096] */
    erayear = (eraday - eraday / (DAYS_PER_4_YEARS - 1) + eraday / DAYS_PER_CENTURY - eraday / (DAYS_PER_ERA - 1)) / 365;	/* [0, 399] */
    yearday = eraday - (DAYS_PER_YEAR * erayear + erayear / 4 - erayear / 100);	/* [0, 365] */
    month = (5 * yearday + 2) / 153;	/* [0, 11] */
    day = yearday - (153 * month + 2) / 5 + 1;	/* [1, 31] */
    month += month < 10 ? 2 : -10;
    year = ADJUSTED_EPOCH_YEAR + erayear + era * YEARS_PER_ERA + (month <= 1);

    res->tm_yday = yearday >= DAYS_PER_YEAR - DAYS_IN_JANUARY - DAYS_IN_FEBRUARY ?
        yearday - (DAYS_PER_YEAR - DAYS_IN_JANUARY - DAYS_IN_FEBRUARY) :
        yearday + DAYS_IN_JANUARY + DAYS_IN_FEBRUARY + _ISLEAP_MK(erayear);

    res->tm_year = year - YEAR_BASE;
    res->tm_mon = month;
    res->tm_mday = day;

    return (res);
}