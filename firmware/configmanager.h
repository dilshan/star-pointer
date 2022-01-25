#ifndef __STARPOINTER_CONFIG_MANAGER_HEADER__
#define __STARPOINTER_CONFIG_MANAGER_HEADER__

#include <inttypes.h>

#include "common.h"

float getLocationDecAngle();
void getLocationLatLng(float *lat, float *lng);
float getInclinationOffset();

void setLocationDecAngle(float decAngle);
void setLocationLatLng(float lat, float lng);
void setInclinationOffset(float offset);

#endif /* __STARPOINTER_CONFIG_MANAGER_HEADER__ */