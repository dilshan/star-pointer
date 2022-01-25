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