#ifndef __STARPOINTER_ADXL345_HEADER__
#define __STARPOINTER_ADXL345_HEADER__

#include "common.h"

uint8_t initAccelerometer();
void readAccelerometerRaw(RawAccData *data);
void readAccelerometerScaled(AccelerometerData *data);

#endif /* __STARPOINTER_ADXL345_HEADER__ */