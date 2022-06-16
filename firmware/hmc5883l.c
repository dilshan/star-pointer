/**********************************************************************************
  StarPointer - Firmware for STM32F103C8.
  HMC5883L - 3-axis digital compass driver.

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

#include <math.h>
#include <stdio.h>

#include "i2c.h"
#include "hmc5883l.h"
#include "systick.h"
#include "logger.h"
#include "common.h"

#define HMC5883L_ADDRESS 0x1E

#define HMC5883L_CONFIG_REG_A   0x00
#define HMC5883L_CONFIG_REG_B   0x01
#define HMC5883L_MODE_REG       0x02
#define HMC5883L_X_DATA_MSB_REG 0x03

#define MEASUREMENT_MODE_CONTINUOUS     0x00
#define MEASUREMENT_MODE_SINGLE         0x01
#define MEASUREMENT_MODE_IDLE           0x03

#define IS_NEAR(a,b) (fabs(a - b) < 0.001)
#define setMeasurementMode(m) writeToMagnetometer(HMC5883L_MODE_REG, m)

static float refGauss[8] = {0.88, 1.30, 1.90, 2.50, 4.00, 4.70, 5.60, 8.10};
static float refScale[8] = {0.73, 0.92, 1.22, 1.52, 2.27, 2.56, 3.03, 4.35};

static float magScale;

static void writeToMagnetometer(uint8_t regAddr, uint8_t value)
{
    uint8_t initCmd[2];
    initCmd[0] = regAddr;
    initCmd[1] = value;
    
    // Update registry with the specified value.
    i2cSendStart();
    i2cSendAddress(HMC5883L_ADDRESS, I2C_WRITE);
    i2cSendData(initCmd, 2);
    i2cSendStop();
}

static uint8_t setScale(float gauss, float *newScale)
{
    uint8_t regVal;

    for(regVal = 0; regVal < 8; regVal++)
    {
        if(IS_NEAR(gauss, refGauss[regVal]))
        {
            *newScale = refScale[regVal];
            writeToMagnetometer(HMC5883L_CONFIG_REG_B, (regVal << 5));
            return SUCCESS;
        }
    }

    LOG("ERROR: Unable to find the matching scale!");
    return FAIL;
}

static uint8_t setSampleRate(uint8_t avgMode)
{
    uint8_t regVal;

    switch (avgMode)
    {
        case 1:
            regVal = 0x10;
            break;
        case 2:
            regVal = 0x30;
            break;
        case 4:
            regVal = 0x50;
            break;    
        case 8:
            regVal = 0x80;
            break;  
        default:
            LOG("ERROR: Specified mode is unsupported or invalid");
            return FAIL;
    }

    writeToMagnetometer(HMC5883L_CONFIG_REG_A, regVal);
}

void readMagnetometerRaw(RawCompassData *data)
{
    uint8_t readCmd[1] = { HMC5883L_X_DATA_MSB_REG };
    uint8_t tempData;

    i2cSendStart();
    i2cSendAddress(HMC5883L_ADDRESS, I2C_WRITE);
    i2cSendData(readCmd, 1);
    i2cSendStop();  

    i2cSendStart();
    i2cSendAddress(HMC5883L_ADDRESS, I2C_READ);
    
    // Read X axis.
    i2cReadByte(&tempData, I2C_READ_ACK);
    data->x = ((uint16_t)tempData) << 8;
    i2cReadByte(&tempData, I2C_READ_ACK);
    data->x |= tempData;

    // Read Z axis.
    i2cReadByte(&tempData, I2C_READ_ACK);
    data->z = ((uint16_t)tempData) << 8;
    i2cReadByte(&tempData, I2C_READ_ACK);
    data->z |= tempData;

    // Read Y axis.
    i2cReadByte(&tempData, I2C_READ_ACK);
    data->y = ((uint16_t)tempData) << 8;
    i2cReadByte(&tempData, I2C_READ_NACK);
    data->y |= tempData;

    i2cSendStop();
}

void readMagnetometerNormalize(CompassData *data)
{
    RawCompassData tempData;
    readMagnetometerRaw(&tempData);

    //LOG("Magnetometer X = %d  Y = %d  Z = %d", tempData.x, tempData.y, tempData.z);

    data->x = ((float)tempData.x) * magScale;
    data->y = ((float)tempData.y) * magScale;
    data->z = ((float)tempData.z) * magScale;
}

void initMagnetometer()
{
    // Set scale of the compass.
    if(setScale(MAGNETOMETER_SCALE, &magScale) == FAIL)
    {
        return;
    }

    LOG("Compass scale = %f", magScale);
    systickDelay(5);

    // Set measurement mode to the continues.
    setMeasurementMode(MEASUREMENT_MODE_CONTINUOUS);

    // Wait minimum of 6ms to settle HMC5883L.
    systickDelay(10);
}