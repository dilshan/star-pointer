#include <stdio.h>

#include "i2c.h"
#include "adxl345.h"
#include "systick.h"
#include "logger.h"
#include "common.h"

#define ADXL345_ADDRESS 0x53

#define ADXL345_DEVID_REG           0x00
#define ADXL345_POWER_CTL_REG       0x2D
#define ADXL345_DATA_FORMAT_REG     0x31
#define ADXL345_BW_RATE_REG         0x2C
#define ADXL345_THRESH_TAP_REG      0x1D
#define ADXL345_DUR_REG             0x21
#define ADXL345_LATENT_REG          0x22
#define ADXL345_WINDOW_REG          0x23
#define ADXL345_THRESH_ACT_REG      0x24
#define ADXL345_THRESH_INACT_REG    0x25
#define ADXL345_TIME_INACT_REG      0x26
#define ADXL345_ACT_INACT_CTL_REG   0x27
#define ADXL345_THRESH_FF_REG       0x28
#define ADXL345_TIME_FF_REG         0x29
#define ADXL345_TAP_AXES            0x2A
#define ADXL345_DATAX0_REG          0x32
#define ADXL345_DATAX1_REG          0x33
#define ADXL345_DATAY0_REG          0x34
#define ADXL345_DATAY1_REG          0x35
#define ADXL345_DATAZ0_REG          0x36
#define ADXL345_DATAZ1_REG          0x37

#define ADXL345_RANGE_2G    0x00
#define ADXL345_RANGE_4G    0x01
#define ADXL345_RANGE_8G    0x02
#define ADXL345_RANGE_16G   0x03

// Set data rate to 100Hz.
#define ADXL345_DATARATE    0x0A
#define ADXL345_RANGE       ADXL345_RANGE_16G

static uint8_t getByteFromReg(uint8_t reg)
{
    uint8_t regBuffer[1];
    uint8_t result;
    
    i2cSendStart();
    i2cSendAddress(ADXL345_ADDRESS, I2C_WRITE);

    regBuffer[0] = reg;
    i2cSendData(regBuffer, 1);
    i2cSendStop();

    i2cSendStart();
    i2cSendAddress(ADXL345_ADDRESS, I2C_READ);
    i2cReadByte(&result, I2C_READ_NACK);
    i2cSendStop();

    return result;
}

static uint16_t getWordFromReg(uint8_t reg)
{
    uint8_t regBuffer[1], tempLow, tempHigh;

    i2cSendStart();
    i2cSendAddress(ADXL345_ADDRESS, I2C_WRITE);

    regBuffer[0] = reg;
    i2cSendData(regBuffer, 1);
    i2cSendStop();

    i2cSendStart();
    i2cSendAddress(ADXL345_ADDRESS, I2C_READ);
    i2cReadByte(&tempLow, I2C_READ_ACK);
    i2cReadByte(&tempHigh, I2C_READ_NACK);
    i2cSendStop();

    return (uint16_t)(tempHigh << 8) | tempLow;
}

static void writeToAccelerometer(uint8_t reg, uint8_t value)
{
    uint8_t regBuffer[2];
    regBuffer[0] = reg;
    regBuffer[1] = value;

    i2cSendStart();
    i2cSendAddress(ADXL345_ADDRESS, I2C_WRITE);
    i2cSendData(regBuffer, 2);
    i2cSendStop();
}

uint8_t initAccelerometer()
{
    uint8_t temp;
    
    // Verify accelerometer is ADXL345.
    if(getByteFromReg(ADXL345_DEVID_REG) != 0xE5)
    {
        LOG("ERROR: Fail to initialize the accelerometer");
        return FAIL;
    }

    // Enable measurement mode.
    writeToAccelerometer(ADXL345_POWER_CTL_REG, 0x08);

    // Initialize ADXL345 by setup the range and resolution.
    temp = getByteFromReg(ADXL345_DATA_FORMAT_REG);
    temp &= 0xF0;
    temp |= (ADXL345_RANGE) | (0x08);
    writeToAccelerometer(ADXL345_DATA_FORMAT_REG, temp);

    // Set data rate of the ADXL345.
    writeToAccelerometer(ADXL345_BW_RATE_REG, ADXL345_DATARATE);

    // Reset or clear registers of the ADXL345.
    writeToAccelerometer(ADXL345_THRESH_TAP_REG, 0x00);
    writeToAccelerometer(ADXL345_DUR_REG, 0x00);
    writeToAccelerometer(ADXL345_LATENT_REG, 0x00);
    writeToAccelerometer(ADXL345_WINDOW_REG, 0x00);
    writeToAccelerometer(ADXL345_THRESH_ACT_REG, 0x00);
    writeToAccelerometer(ADXL345_THRESH_INACT_REG, 0x00);
    writeToAccelerometer(ADXL345_TIME_INACT_REG, 0x00);

    temp = getByteFromReg(ADXL345_ACT_INACT_CTL_REG);
    temp &= 0x88;
    writeToAccelerometer(ADXL345_ACT_INACT_CTL_REG, temp);

    writeToAccelerometer(ADXL345_THRESH_FF_REG, 0x00);
    writeToAccelerometer(ADXL345_TIME_FF_REG, 0x00);

    temp = getByteFromReg(ADXL345_TAP_AXES);
    temp &= 0xF8;
    writeToAccelerometer(ADXL345_TAP_AXES, temp);

    return SUCCESS;
}

void readAccelerometerRaw(RawAccData *data)
{
    data->x = getWordFromReg(ADXL345_DATAX0_REG);
    data->y = getWordFromReg(ADXL345_DATAY0_REG);
    data->z = getWordFromReg(ADXL345_DATAZ0_REG);
}

void readAccelerometerScaled(AccelerometerData *data)
{
    RawAccData tempData;
    readAccelerometerRaw(&tempData);

    data->x = tempData.x * 0.004;
    data->y = tempData.y * 0.004;
    data->z = tempData.z * 0.004;
}
