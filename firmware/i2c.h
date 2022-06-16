/**********************************************************************************
  StarPointer - Firmware for STM32F103C8.
  LibOpenCM3 based I2C driver for STM32F103C8.

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

#ifndef __STARPOINTER_I2C_HEADER__
#define __STARPOINTER_I2C_HEADER__

#include <libopencm3/stm32/i2c.h>

#define I2C_TIMEOUT_MS  100

#define I2C_READ_ACK    1
#define I2C_READ_NACK   0

void initI2C(void);
void i2cSendStart(void);
void i2cSendStop(void);

// In i2cSendAddress use I2C_READ or I2C_WRITE for the readwrite flag.
void i2cSendAddress(uint8_t address, uint8_t readwrite);
void i2cSendData(uint8_t *buffer, size_t bufferSize);
uint8_t i2cReadByte(uint8_t *data, uint8_t ack);

#endif /* __STARPOINTER_I2C_HEADER__ */