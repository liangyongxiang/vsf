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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_test_usart_mode.h"
#include "service/trace/vsf_trace.h"

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MODE_PAYLOAD
#   define VSF_TEST_MODE_PAYLOAD            VSF_TEST_USART_TX_MODE_PAYLOAD
#endif
#ifndef VSF_TEST_MODE_PAYLOAD_DRAIN_MS
#   define VSF_TEST_MODE_PAYLOAD_DRAIN_MS   VSF_TEST_USART_TX_MODE_PAYLOAD_DRAIN_MS
#endif
#ifndef VSF_TEST_MODE_DEFAULT_BAUDRATE
#   define VSF_TEST_MODE_DEFAULT_BAUDRATE    115200
#endif

/*============================ LOCAL VARIABLES ===============================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __usart_send_str(vsf_usart_t *usart, const char *str)
{
    while (*str) {
        uint32_t spin = 0;
        while (!vsf_usart_txfifo_get_free_count(usart)) {
            if (++spin > 10000000) {
                vsf_usart_status_t status = vsf_usart_status(usart);
                vsf_trace_error("[TXSTALL] status=0x%08X free=%u\r\n",
                                (unsigned)status.value,
                                (unsigned)vsf_usart_txfifo_get_free_count(usart));
                return;
            }
        }
        vsf_usart_txfifo_write(usart, (uint8_t *)str, 1);
        str++;
    }
}

/*============================ IMPLEMENTATION ================================*/

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
        /* Poll TX FIFO empty instead of fixed busy_wait.
         * The prior timer-based busy_wait intermittently dead-locked on
         * CASE:9 (root cause unknown — timer appeared to stop). */
        {
            vsf_usart_status_t status;
            uint32_t poll = 0;
            do {
                status = vsf_usart_status(c->suite->usart);
            } while (status.is_busy && ++poll < 10000000);
        }
        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->suite->usart);
}

#endif /* VSF_TEST_USART_TX_MODE_ENABLE == ENABLED */

/* EOF */
