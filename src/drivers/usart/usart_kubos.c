/*
 * KubOS HAL
 * Copyright (C) 2016 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include "kubos-hal/uart.h"
#include <stdlib.h>

KUARTNum uart; /* global device num */
usart_callback_t usart_callback;

void usart_init(struct usart_conf *conf)
{
    /* check if conf is valid */
    if (conf == NULL)
    {
        return;
    }

    /* set dev num */
    uart = (KUARTNum)*(conf->device);

    /* set params for k_uart */
    KUARTConf k_uart_conf = {
            .baud_rate = conf->baudrate,
            .word_len = conf->databits,
            .stop_bits = conf->stopbits,
            .parity = conf->paritysetting,
            .rx_queue_len = YOTTA_CFG_HARDWARE_UARTDEFAULTS_RXQUEUELEN,
            .tx_queue_len = YOTTA_CFG_HARDWARE_UARTDEFAULTS_TXQUEUELEN,
        };

    /* initialize kubos uart */
    k_uart_init(uart, &k_uart_conf);
}

void usart_init_default(void)
{
    /* set default device as char */
    char dev = (char)YOTTA_CFG_HARDWARE_KISS_UART;

    usart_conf conf = {
        .device = &dev, /* pointer to device */
        .baudrate = YOTTA_CFG_HARDWARE_UARTDEFAULTS_BAUDRATE,
        .databits = YOTTA_CFG_HARDWARE_UARTDEFAULTS_WORDLEN,
        .stopbits = YOTTA_CFG_HARDWARE_UARTDEFAULTS_STOPBITS,
        .paritysetting = YOTTA_CFG_HARDWARE_UARTDEFAULTS_PARITY,
    };

    /* initialize */
    usart_init(&conf);
}

void usart_set_callback(usart_callback_t callback)
{
    usart_callback = callback;
}

void usart_insert(char c, void *pxTaskWoken)
{
    /* put back into UART rx queue */
    k_uart_rx_queue_push(uart, c, pxTaskWoken);
}

void usart_putc(char c)
{
    k_uart_write(uart, &c, 1);
}

void usart_putstr(char *buf, int len)
{
    k_uart_write(uart, buf, len);
}

char usart_getc(void)
{
    char buf = 0;
    k_uart_read(uart, &buf, 1);
    return buf;
}

int usart_messages_waiting(int handle)
{
    /* get rx queue len */
    return k_uart_rx_queue_len((KUARTNum) handle);
}
