#include "common.h"
#include "configmanager.h"

void getLocationDecAngle(Angle *decLoc)
{
    decLoc->deg = 0;
    decLoc->min = 0;
    decLoc->sec = 0;
}

void getLocationLatLng(float *lat, float *lng)
{
    *lat = 6.8511;
    *lng = 79.9212;
}