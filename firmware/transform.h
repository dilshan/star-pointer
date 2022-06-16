/**********************************************************************************
  StarPointer - Firmware for STM32F103C8.
  Data transformation utilities.

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

#ifndef __STARPOINTER_TRANSFORM_UTIL_HEADER__
#define __STARPOINTER_TRANSFORM_UTIL_HEADER__

#include <inttypes.h>

#include "common.h"

void convertAngleToInt(float angle, Angle *convAngle);
float convertIntToAngle(Angle *convAngle);

float fixAngle(float angle);

float fixTiltCompensate(CompassData *compData, AccelerometerData *accData);
float getPitchFromAccelerometer(AccelerometerData *accData, float offset);

void convertAzAltToRaDec(float alt, float az, uint16_t year, uint8_t month, uint8_t date, 
    float uniTime, float lat, float lng, float *resultRA, float *resultDEC);

float convertTimeToFloat(struct tm *dateTime);

#endif /* __STARPOINTER_TRANSFORM_UTIL_HEADER__ */