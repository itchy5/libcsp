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
#include <stdlib.h>
#include <csp/interfaces/csp_if_kubos.h>
#include <csp/interfaces/csp_if_kiss.h>

/* static  CSP interface */
static csp_iface_t csp_if_kiss;
static csp_kiss_handle_t csp_kiss_driver;

void k_init_csp(k_csp_driver driver)
{
    switch(driver)
    {
        case CSP_KISS:
        {
            k_init_kiss_csp();
            break;
        }
        case CSP_I2C:
        {
            /* I2C init function goes here */
            break;
        }
        case CSP_CAN:
        {
            /* CAN init function goes here */
            break;
        }
        default:
        {
        }
    }
}

void k_init_kiss_csp(void)
{
    /* init kiss interface */
    csp_kiss_init(&csp_if_kiss, &csp_kiss_driver, usart_putc, usart_insert, "KISS");

    /* Setup callback from USART RX to KISS RS */
    void my_usart_rx(uint8_t * buf, int len, void * pxTaskWoken) {
        csp_kiss_rx(&csp_if_kiss, buf, len, pxTaskWoken);
    }
    usart_set_callback(my_usart_rx);

    /* csp buffer and mtu in csp_iface must match */
    csp_buffer_init(5, 256);
    csp_init(MY_ADDRESS);
    /* set to route through KISS / UART */
    csp_route_set(OTHER_ADDRESS, &csp_if_kiss, CSP_NODE_MAC);
    csp_route_start_task(500, 1);
}
