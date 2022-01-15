#ifndef __STARPOINTER_TRANSFORM_UTIL_HEADER__
#define __STARPOINTER_TRANSFORM_UTIL_HEADER__

#include <inttypes.h>

#include "common.h"

void convertAngleToInt(float angle, Angle *convAngle);
float convertIntToAngle(Angle *convAngle);

float fixAngle(float angle);

float fixTiltCompensate(CompassData *compData, AccelerometerData *accData);
float getPitchFromAccelerometer(AccelerometerData *accData);

void convertAzAltToRaDec(float alt, float az, uint16_t year, uint8_t month, uint8_t date, 
    float uniTime, float lat, float lng, float *resultRA, float *resultDEC);

float convertTimeToFloat(struct tm *dateTime);

#endif /* __STARPOINTER_TRANSFORM_UTIL_HEADER__ */