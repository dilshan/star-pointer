/**********************************************************************************
  StarPointer - Firmware for STM32F103C8.
  Common data structures.

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