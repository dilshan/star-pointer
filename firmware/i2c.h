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