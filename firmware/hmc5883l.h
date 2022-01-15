#ifndef __STARPOINTER_HMC5883L_HEADER__
#define __STARPOINTER_HMC5883L_HEADER__

#include "common.h"

#define MAGNETOMETER_SCALE  1.3

void initMagnetometer(void);
void readMagnetometerRaw(RawCompassData *data);
void readMagnetometerNormalize(CompassData *data);

#endif /* __STARPOINTER_HMC5883L_HEADER__ */