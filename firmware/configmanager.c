/**********************************************************************************
  StarPointer - Firmware for STM32F103C8.
  Configuration manager for STM32F103C8.

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