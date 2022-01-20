#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "transform.h"
#include "common.h"
#include "logger.h"

void convertAngleToInt(float angle, Angle *convAngle)
{
    float absDiff;

    // Calculate angle in degrees with sign.
    convAngle->deg = (int)angle;
    absDiff = fabs(angle - convAngle->deg);

    // Calculate minutes and seconds without sign.
    convAngle->min = (int)(absDiff * 60.0);
    convAngle->sec = (int)((absDiff - (convAngle->min * (1.0/60.0))) * 3600.0);
}

float convertIntToAngle(Angle *convAngle)
{
    return convAngle->deg + (convAngle->min/60.0) + (convAngle->sec/3600.0);
}

float fixTiltCompensate(CompassData *compData, AccelerometerData *accData)
{
    float roll, pitch, tiltX, tiltY;

    roll = asin(accData->y);
    pitch = asin(-(accData->x));

    if ((roll > 0.78) || (roll < -0.78) || (pitch > 0.78) || (pitch < -0.78))
    {
        return atan2(compData->y, compData->x);
    }

    tiltX = compData->x * cos(pitch) + compData->z * sin(pitch);
    tiltY = compData->x * sin(roll) * sin(pitch) + compData->y * cos(roll) - compData->z * sin(roll) * cos(pitch);

    return atan2(tiltY, tiltX);
}

float fixAngle(float angle)
{
    if(angle < 0)
    {
        angle += (2.0 * PI);
    }

    if(angle > (2 * PI))
    {
        angle -= (2.0 * PI);
    }

    return angle;
}

float getPitchFromAccelerometer(AccelerometerData *accData)
{
    float pitch = atan2(accData->x, sqrt(accData->y*accData->y + accData->z*accData->z));
    if(accData->z < 0)
    {
        pitch = PI - pitch;
    }
    
    return fixAngle(pitch) * 180.0 / PI;
}

// TODO: Optimize to avoid recalculations.
void convertAzAltToRaDec(float alt, float az, uint16_t year, uint8_t month, uint8_t date, float uniTime, float lat, float lng, float *resultRA, float *resultDEC)
{
    float julianDate, gmTime, gmSiderealTime, localSiderealTime, altr, azr, latr;
    float decr, har, ra, hrAngle, localSiderealTimeDeg;

    // Based on http://www.ras.ucalgary.ca/grad_project_2005/pointing.shtml
    // Calculate Julian Date in J2000 format.
    julianDate = (367 * year) - floor( 7 *(year + floor((month + 9.0)/12.0))/4.0) + floor(275 * month/9.0) + date - 730531.5 + (uniTime/24.0); 
    //LOG("julianDate = %f", julianDate);

    gmTime = 18.697374558 + 24.06571* julianDate;
    gmSiderealTime = fmod(gmTime, 24.0);
    localSiderealTime = gmSiderealTime + (lng/15.0);

    if(localSiderealTime < 0)
    {
        localSiderealTime += 24.0;
    }

    // Converting Altitude, Azimuth and Latitude to radians.
    altr = alt * PI/180.0;
    azr = az * PI/180.0;
    latr = lat * PI/180.0;

    // Calculate the DEC and hour angle.
    decr = asin((cos(azr)*cos(latr)*cos(altr))+(sin(latr)*sin(altr)));
    har = acos((sin(altr)-sin(decr)*sin(latr))/(cos(decr)*cos(latr)));

    // Convert hour angle to degrees.    
    hrAngle = har * 180.0/PI;

    if(az < 180)
    {
        hrAngle = hrAngle * -1;
    }

    // Convert the local mean sidereal time to degrees.
    localSiderealTimeDeg = localSiderealTime * 360/24;

    // Calculate RA.
    ra = localSiderealTimeDeg - hrAngle; 
    if(ra > 360)
    {
        ra = ra - 360.0;
    }

    // Convert RA and DEC to degrees.
    *resultRA = (float)(ra * 24.0/360.0);
    *resultDEC = (float)(decr * 180.0/PI);
}

float convertTimeToFloat(struct tm *dateTime)
{
    return (dateTime->tm_hour + ((float)(dateTime->tm_min)/60.0) + ((float)(dateTime->tm_sec)/3600.0));
}