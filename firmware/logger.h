/**********************************************************************************
  StarPointer - Firmware for STM32F103C8.
  USART based logger functions.

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

#define LOG_TIME(t) LOG("TIME = %d:%d:%d", t.tm_hour, t.tm_min, t.tm_sec);
#define LOG_DATE(t)	LOG("DATE = %d-%d-%d", t.tm_mday, t.tm_mon, t.tm_year);

#endif /* __STARPOINTER_LOGGER_HEADER__ */