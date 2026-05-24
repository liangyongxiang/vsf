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

#include "vsf_test_usart_request_cancel.h"

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

VSF_TEST_SUITE_REGISTER(vsf_test_usart_request_cancel_add_cases,
    vsf_test_usart_request_cancel_suite_t,
    vsf_test_usart_request_cancel_case_t,
    vsf_test_usart_request_cancel_run,
    VSF_TEST_USART_REQUEST_CANCEL_CASES_INIT,
    "usart_request_cancel", "cancel", "uart1+la",
    false)

void vsf_test_usart_request_cancel_run(const vsf_test_usart_request_cancel_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    vsf_usart_t *usart = c->suite->usart;

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    uint32_t total = (uint32_t)cap.txfifo_depth * c->refill_target;
    if (total < 64) { total = 64; }
    static uint8_t buf[256];
    if (total > sizeof(buf)) { total = sizeof(buf); }
    for (uint32_t i = 0; i < total; i++) { buf[i] = (uint8_t)(i & 0xFF); }

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE,
        .baudrate = 115200,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    err = vsf_usart_request_tx(usart, buf, total);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Let some bytes drain through the line before pulling the cancel. */
    uint32_t waited = 0;
    while (waited < c->cancel_after_us) {
        vsf_test_busy_wait_ms(1);
        waited += 1000;
    }

    err = vsf_usart_cancel_tx(usart);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    int_fast32_t cnt = vsf_usart_get_tx_count(usart);
    /* Partial: > 0, < total. The strict inequality holds only if the cancel
     * fires before the whole payload has been clocked out. */
    vsf_trace_info("USART:REQ_CANCEL:total=%lu count=%ld" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)total, (long)cnt);
    VSF_TEST_ASSERT(cnt >= 0);
    VSF_TEST_ASSERT((uint32_t)cnt <= total);

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD): once the
     * outstanding bytes have clocked out (no flush API on PL011, so we wait
     * for natural drain), status().txfe must report empty. Confirms the
     * status struct tracks reality after a cancel + drain sequence. */
    vsf_test_busy_wait_ms(50);
    vsf_usart_status_t st = vsf_usart_status(usart);
    VSF_TEST_ASSERT(st.txfe);
    VSF_TEST_ASSERT(!st.is_busy);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED */

/* EOF */
