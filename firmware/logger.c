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

#include <string.h>
#include <stdio.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>

#include "logger.h"
#include "common.h"

int8_t logBuffer[LOG_BUFFER_SIZE];

void initLogger()
{
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

    usart_set_baudrate(USART2, LOGGER_BAUD_RATE);
    usart_set_databits(USART2, 8);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_set_mode(USART2, USART_MODE_TX);

    usart_enable(USART2);
}

void sendLog(int8_t *buffer)
{
    size_t strPos, strLength;
    
    if(buffer != NULL)
    {
        strLength = strlen(buffer);

        // Send entier log message buffer into the host terminal.
        for(strPos = 0; strPos < strLength; strPos++)
        {
            usart_send_blocking(USART2, buffer[strPos]);
        }

        // Add new line to the end of message buffer.
        usart_send_blocking(USART2, '\r');
        usart_send_blocking(USART2, '\n');
    }
}