#include <libopencm3/stm32/rcc.h>

#include "common.h"
#include "configmanager.h"

typedef union configData
{
    float angle;
    uint32_t angleData;
} configData;

void setBackupReg(volatile int32_t *reg1, volatile int32_t *reg2, float angle)
{
    configData tempData;
    
    tempData.angle = angle;
    
    *reg1 = tempData.angleData & 0xFFFF;
    *reg2 = (tempData.angleData >> 16) & 0xFFFF;
}

float getBackupReg(volatile int32_t *reg1, volatile int32_t *reg2)
{
    configData tempData;

    tempData.angleData = (*reg1) | ((*reg2) << 16);    
    return tempData.angle;
}

void getLocationLatLng(float *lat, float *lng)
{
    configData tempData;

    *lat = getBackupReg(&BKP_DR4, &BKP_DR5);
    *lng = getBackupReg(&BKP_DR6, &BKP_DR7);
}

void setLocationLatLng(float lat, float lng)
{
    setBackupReg(&BKP_DR4, &BKP_DR5, lat);
    setBackupReg(&BKP_DR6, &BKP_DR7, lng);
}