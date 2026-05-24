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

#include "vsf_test_usart_baud.h"

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_BAUD_PAYLOAD
#   define VSF_TEST_BAUD_PAYLOAD            "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_BAUD_PAYLOAD_DRAIN_MS
#   define VSF_TEST_BAUD_PAYLOAD_DRAIN_MS   500
#endif
#ifndef VSF_TEST_BAUD_DEFAULT_MODE
#   define VSF_TEST_BAUD_DEFAULT_MODE        (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_TX_ENABLE)
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_usart_baud_case_t __baud_cases[] = {
    VSF_TEST_USART_TX_BAUD_CASES_INIT
};

/*============================ LOCAL FUNCTIONS ===============================*/

static void __usart_send_str(vsf_usart_t *usart, const char *str)
{
    while (*str) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)str, 1);
        str++;
    }
}

static void __usart_send_bulk(vsf_usart_t *usart, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        uint8_t b = (uint8_t)(i & 0xFF);
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, &b, 1);
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_baud_add_cases(vsf_test_usart_baud_suite_t *suite)
{
    suite->name    = "usart_baud";
    suite->purpose = "tx-baud";
    suite->hw_req  = "uart1+la";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_USART_TX_BAUD_CASE_COUNT; i++) {
        __baud_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_baud_run,
            (void *)&__baud_cases[i]);
    }
}

void vsf_test_usart_baud_run(const vsf_test_usart_baud_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    vsf_usart_capability_t cap = vsf_usart_capability(c->suite->usart);
    bool expect_pass = (c->baudrate >= cap.min_baudrate)
                    && (c->baudrate <= cap.max_baudrate)
                    && (c->baudrate != 0);

    vsf_err_t err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_BAUD_DEFAULT_MODE,
        .baudrate = c->baudrate,
    });

    if (expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));

        /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
         * get_configuration() must round-trip the values we passed to init().
         * Catches drivers that "accept" a baudrate but quietly clamp or drop
         * mode bits without telling the caller. */
        vsf_usart_cfg_t got = {0};
        vsf_err_t cfg_err = vsf_usart_get_configuration(c->suite->usart, &got);
        if (cfg_err == VSF_ERR_NONE) {
            VSF_TEST_ASSERT(got.baudrate == c->baudrate);
            VSF_TEST_ASSERT(got.mode == VSF_TEST_BAUD_DEFAULT_MODE);
        }

        if (c->data_size_bytes > 0) {
            __usart_send_bulk(c->suite->usart, c->data_size_bytes);
            /* Scale drain: 10 bits/byte @ baudrate, ms = bytes * 10 * 1000 / baudrate */
            uint32_t drain_ms = (c->data_size_bytes * 10 * 1000) / c->baudrate;
            if (drain_ms < 100) { drain_ms = 100; }
            vsf_test_busy_wait_ms(drain_ms);
        } else {
            __usart_send_str(c->suite->usart, VSF_TEST_BAUD_PAYLOAD);
            vsf_test_busy_wait_ms(VSF_TEST_BAUD_PAYLOAD_DRAIN_MS);
        }

        /* Phase-3 API completeness check: after TX drain, status() must
         * report tx-fifo-empty and not-busy. Catches drivers that never
         * update status() after a TX completes. */
        vsf_usart_status_t st = vsf_usart_status(c->suite->usart);
        VSF_TEST_ASSERT(st.txfe);
        VSF_TEST_ASSERT(!st.is_busy);

        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->suite->usart);

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
     * fini/disable lifecycle — after a full disable+fini, a second init+
     * enable+disable+fini cycle must succeed. Catches drivers that leak
     * state between init() calls or fail to reset the peripheral on fini. */
    if (expect_pass) {
        err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
            .mode     = VSF_TEST_BAUD_DEFAULT_MODE,
            .baudrate = c->baudrate,
        });
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));
        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
        vsf_usart_fini(c->suite->usart);
    }
}

#endif /* VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED */

/* EOF */
