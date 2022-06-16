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

#ifndef __STARPOINTER_CONFIG_MANAGER_HEADER__
#define __STARPOINTER_CONFIG_MANAGER_HEADER__

#include <inttypes.h>

#include <libopencm3/stm32/f1/bkp.h>

#include "common.h"

#define setLocationDecAngle(f) setBackupReg(&BKP_DR2, &BKP_DR3, f)
#define setInclinationOffset(f) setBackupReg(&BKP_DR8, &BKP_DR9, f)

#define getLocationDecAngle() getBackupReg(&BKP_DR2, &BKP_DR3)
#define getInclinationOffset() getBackupReg(&BKP_DR8, &BKP_DR9)

float getBackupReg(volatile int32_t *reg1, volatile int32_t *reg2);
void getLocationLatLng(float *lat, float *lng);

void setBackupReg(volatile int32_t *reg1, volatile int32_t *reg2, float angle);
void setLocationLatLng(float lat, float lng);

#endif /* __STARPOINTER_CONFIG_MANAGER_HEADER__ */