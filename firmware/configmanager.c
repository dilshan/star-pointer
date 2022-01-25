#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/bkp.h>

#include "common.h"
#include "configmanager.h"

typedef union configData
{
    float angle;
    uint32_t angleData;
} configData;

static void setBackupReg(volatile int32_t *reg1, volatile int32_t *reg2, configData val)
{
    *reg1 = val.angleData & 0xFFFF;
    *reg2 = (val.angleData >> 16) & 0xFFFF;
}

void getLocationLatLng(float *lat, float *lng)
{
    configData tempData;

    tempData.angleData = BKP_DR4 | (BKP_DR5 << 16);
    *lat = tempData.angle;

    tempData.angleData = BKP_DR6 | (BKP_DR7 << 16);
    *lng = tempData.angle;
}

void setLocationLatLng(float lat, float lng)
{
    configData tempData;

    tempData.angle = lat;
    setBackupReg(&BKP_DR4, &BKP_DR5, tempData);

    tempData.angle = lng;
    setBackupReg(&BKP_DR6, &BKP_DR7, tempData);
}

float getLocationDecAngle()
{
    configData tempData;

    tempData.angleData = BKP_DR2 | (BKP_DR3 << 16);    
    return tempData.angle;
}

void setLocationDecAngle(float decAngle)
{
    configData tempData;
    
    tempData.angle = decAngle;
    setBackupReg(&BKP_DR2, &BKP_DR3, tempData);
}

float getInclinationOffset()
{
    configData tempData;

    tempData.angleData = BKP_DR8 | (BKP_DR9 << 16);    
    return tempData.angle;
}

void setInclinationOffset(float offset) 
{
    configData tempData;
    
    tempData.angle = offset;
    setBackupReg(&BKP_DR8, &BKP_DR9, tempData);
}