#ifndef __STARPOINTER_COM_PROTOCOL_HEADER__
#define __STARPOINTER_COM_PROTOCOL_HEADER__

#include <inttypes.h>

#include "common.h"

void extractDateInfo(uint8_t *buffer, struct tm *dateTimeInfo);
void extractTimeInfo(uint8_t *buffer, struct tm *dateTimeInfo);
float extractAngle(uint8_t *buffer);

#endif /* __STARPOINTER_COM_PROTOCOL_HEADER__ */