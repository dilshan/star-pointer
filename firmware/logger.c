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