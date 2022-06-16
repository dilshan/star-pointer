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

#include <stdio.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/cortex.h>

#include "i2c.h"
#include "logger.h"
#include "systick.h"

void initI2C()
{
    i2c_peripheral_disable(I2C2);
    i2c_reset(I2C2);

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_I2C2_SCL | GPIO_I2C2_SDA);
    
    // Set I2C clock frequency to 36MHz.
    i2c_set_clock_frequency(I2C2, 36);
    i2c_set_standard_mode(I2C2);

    // 36MHz / (2 x 100kHz) = 36000000÷200000 = 180 = 0xB4
    i2c_set_ccr(I2C2, 0xB4);

    // (36MHz / 1MHz) + 1 = 36 + 1 = 37 =   0x25 
    i2c_set_trise(I2C2, 0x25);

    // Set I2C slave address as 0x32 (not useful in master mode).
    i2c_set_own_7bit_slave_address(I2C2, 0x32);

    i2c_enable_interrupt(I2C2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
    i2c_peripheral_enable(I2C2);
    i2c_enable_ack(I2C2);
}

void i2cSendStart()
{
    I2C_CR1(I2C2) |= I2C_CR1_ACK;
    i2c_send_start(I2C2);

    // Wait to send the START signal.
    uint32_t startTime = sysTickMs;
    while (!((I2C_SR1(I2C2) & I2C_SR1_SB) & (I2C_SR2(I2C2) & (I2C_SR2_MSL | I2C_SR2_BUSY))))
    {
        if((sysTickMs - startTime) > I2C_TIMEOUT_MS)
        {
            LOG("ERROR: Timeout in i2cSendStart");
            break;
        }
    }
}

void i2cSendAddress(uint8_t address, uint8_t readwrite)
{
    uint16_t dummyVal;
    
    i2c_send_7bit_address(I2C2, address, readwrite);

    // Wait to send the slave address.
    uint32_t startTime = sysTickMs;
    while (!(I2C_SR1(I2C2) & I2C_SR1_ADDR))
    {
        if((sysTickMs - startTime) > I2C_TIMEOUT_MS)
        {
            LOG("ERROR: Timeout in i2cSendAddress");
            break;
        }
    }

    // Clear ADDR flag by reading SR1 and SR2.
    dummyVal = I2C_SR1(I2C2);
    dummyVal = I2C_SR2(I2C2);
}

void i2cSendData(uint8_t *buffer, size_t bufferSize)
{        
    size_t pos;
    uint32_t startTime;
    
    // Send each byte into the slave device.
    for(pos = 0; pos < bufferSize; pos++)
    {
        i2c_send_data(I2C2, buffer[pos]);

        // Wait to send data to the slave device.
        startTime = sysTickMs;
        while (!(I2C_SR1(I2C2) & I2C_SR1_BTF))
        {
            if((sysTickMs - startTime) > I2C_TIMEOUT_MS)
            {
                LOG("ERROR: Timeout in i2cSendData");                
                return;
            }
        }
    }
}

uint8_t i2cReadByte(uint8_t *data, uint8_t ack)
{
    uint32_t startTime, srVal;
    uint8_t result = 0;
    
    I2C_CR1(I2C2) |= I2C_CR1_POS;
    I2C_CR1(I2C2) = (ack) ? (I2C_CR1(I2C2) | I2C_CR1_ACK) : (I2C_CR1(I2C2) & (~I2C_CR1_ACK));

    startTime = sysTickMs;
    while(1)
    {
        srVal = I2C_SR1(I2C2);
        if(srVal & (I2C_SR1_OVR | I2C_SR1_BERR))
        {
            LOG("ERROR: Read error i2cReadByte");   
            result = 1;             
            break;
        }
        else if(srVal & I2C_SR1_RxNE)
        {
            break;
        }

        if((sysTickMs - startTime) > I2C_TIMEOUT_MS)
        {
            LOG("ERROR: Timeout in i2cReadByte");   
            result = 1;             
            break;
        }
    }

    *data = i2c_get_data(I2C2);

    return result;
}

void i2cSendStop()
{
    uint16_t dummyVal;
    
    i2c_send_stop(I2C2);

    dummyVal = I2C_SR1(I2C2);
    dummyVal = I2C_SR2(I2C2);
}