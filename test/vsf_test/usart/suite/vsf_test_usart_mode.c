/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_test_usart_mode.h"

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MODE_PAYLOAD
#   define VSF_TEST_MODE_PAYLOAD            "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_MODE_PAYLOAD_DRAIN_MS
#   define VSF_TEST_MODE_PAYLOAD_DRAIN_MS   500
#endif
#ifndef VSF_TEST_MODE_DEFAULT_BAUDRATE
#   define VSF_TEST_MODE_DEFAULT_BAUDRATE    115200
#endif

/*============================ LOCAL VARIABLES ===============================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __usart_send_str(vsf_usart_t *usart, const char *str)
{
    while (*str) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)str, 1);
        str++;
    }
}

/*============================ IMPLEMENTATION ================================*/

VSF_TEST_SUITE_REGISTER(vsf_test_usart_mode_add_cases,
    vsf_test_usart_mode_suite_t,
    vsf_test_usart_mode_case_t,
    vsf_test_usart_mode_run,
    VSF_TEST_USART_TX_MODE_CASES_INIT,
    "usart_mode", "tx-mode", "uart1+la",
    false)

void vsf_test_usart_mode_run(const vsf_test_usart_mode_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    vsf_err_t err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = VSF_TEST_MODE_DEFAULT_BAUDRATE,
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));
        __usart_send_str(c->suite->usart, VSF_TEST_MODE_PAYLOAD);
        vsf_test_busy_wait_ms(VSF_TEST_MODE_PAYLOAD_DRAIN_MS);
        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->suite->usart);
}

#endif /* VSF_TEST_USART_TX_MODE_ENABLE == ENABLED */

/* EOF */
