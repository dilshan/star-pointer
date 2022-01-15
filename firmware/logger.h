#ifndef __STARPOINTER_LOGGER_HEADER__
#define __STARPOINTER_LOGGER_HEADER__

#include <stdint.h>

#include "common.h"

#define LOGGER_BAUD_RATE    38400
#define LOG_BUFFER_SIZE 42

extern int8_t logBuffer[LOG_BUFFER_SIZE];

void initLogger(void);
void sendLog(int8_t *buffer);

#define LOG(...) { sprintf(logBuffer, __VA_ARGS__); sendLog(logBuffer); }

#endif /* __STARPOINTER_LOGGER_HEADER__ */