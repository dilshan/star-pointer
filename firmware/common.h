#ifndef __STARPOINTER_COMMON_HEADER__
#define __STARPOINTER_COMMON_HEADER__

#include <stdint.h>

#define SUCCESS     0
#define FAIL        1

#define PI      3.1415926535897932
#define GRAVITY 9.80665

#define time_t  uint32_t

typedef struct RawCompassData
{
    int16_t x;
    int16_t y;
    int16_t z;
} RawCompassData;

typedef struct RawAccData
{
    int16_t x;
    int16_t y;
    int16_t z;
} RawAccData;

typedef struct CompassData
{
    float x;
    float y;
    float z;
} CompassData;

typedef struct AccelerometerData
{
    float x;
    float y;
    float z;
} AccelerometerData;

typedef struct Angle
{
    int16_t deg;
    int16_t min;
    int16_t sec;
} Angle;

typedef enum
{
    MSG_IDLE,
    MSG_GET_RA,
    MSG_GET_DEC,
    MSG_SET_DATE,
    MSG_SET_TIME,
    MSG_SET_LAT,
    MSG_SET_LNG,
    MSG_SET_MAG_OFFSET,
    MSG_SET_INCL_OFFSET
} USBMsgState;

// Date time structure used in time.h related functions.
// Original version extracted from Newlib [https://sourceware.org/newlib/].
struct tm
{
    uint8_t tm_sec;
    uint8_t tm_min;
    uint8_t tm_hour;
    uint8_t tm_mday;
    uint8_t tm_mon;
    uint16_t tm_year;
    uint16_t tm_wday;
    uint16_t tm_yday;
};

#endif /* __STARPOINTER_COMMON_HEADER__ */