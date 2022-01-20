#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "transform.h"
#include "logger.h"
#include "protocol.h"

#define MSG_DATA_OFFSET 0x04

void extractDateInfo(uint8_t *buffer, struct tm *dateTimeInfo)
{
    // Command format - #:SCMM/DD/YYYY# 
    // Data - M M / D D / Y Y Y Y    
  
    uint8_t tempBuffer[5];
    
    // Extract and copy month.
    memset(tempBuffer, 0, 5);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET, 2);
    dateTimeInfo->tm_mon = atoi(tempBuffer);

    // Extract and copy date.
    memset(tempBuffer, 0, 5);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET + 3, 2);
    dateTimeInfo->tm_mday = atoi(tempBuffer);

    // Extract and copy year.
    memset(tempBuffer, 0, 5);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET + 6, 4);
    dateTimeInfo->tm_year = atoi(tempBuffer);
}

void extractTimeInfo(uint8_t *buffer, struct tm *dateTimeInfo)
{
    // Command format - #:SLHH:MM:SS#
    // Data - H H : M M : S S   

    uint8_t tempBuffer[3];

    // Extract and copy hour.
    memset(tempBuffer, 0, 3);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET, 2);
    dateTimeInfo->tm_hour = atoi(tempBuffer);
    
    // Extract and copy minutes.
    memset(tempBuffer, 0, 3);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET + 3, 2);
    dateTimeInfo->tm_min = atoi(tempBuffer);

    // Extract and copy seconds. 
    memset(tempBuffer, 0, 3);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET + 6, 2);
    dateTimeInfo->tm_sec = atoi(tempBuffer);
}

float extractAngle(uint8_t *buffer)
{
    // Command format - #:XXsDDD:MM:SS#
    // Data - s D D D : M M : S S

    uint8_t tempBuffer[4];
    Angle tempAngle;

    // Extract degree value from the specified buffer.
    memset(tempBuffer, 0, 4);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET + 1, 3);
    tempAngle.deg = atoi(tempBuffer);

    // Extract minutes component from the specified buffer.
    memset(tempBuffer, 0, 4);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET + 5, 2);
    tempAngle.min = atoi(tempBuffer);

    // Extract seconds component from the specified buffer.
    memset(tempBuffer, 0, 4);
    memcpy(tempBuffer, buffer + MSG_DATA_OFFSET + 8, 2);
    tempAngle.sec = atoi(tempBuffer);

    // Extract sign of the angle.
    if(buffer[MSG_DATA_OFFSET] == '-')
    {
        tempAngle.deg *= -1;
    }

    LOG("ANGLE : %d. %d. %d", tempAngle.deg, tempAngle.min, tempAngle.sec);
    return convertIntToAngle(&tempAngle);
}