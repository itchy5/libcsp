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

/**
 * @defgroup KUBOSInterface
 * @addtogroup KUBOSInterface
 * @{
 */

/* different driver types */
typedef enum {
    CSP_KISS = 0,
    CSP_I2C,
    CSP_CAN,
} k_csp_driver;

/**
 * choose driver to use and init csp routing
 * @param driver is the driver to initialize
 */
void k_init_csp(k_csp_driver driver);

/**
 * initialize kiss/uart interface
 */
void k_init_kiss_csp(void);
