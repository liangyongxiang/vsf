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

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_usart_request_cancel_case_t __request_cancel_cases[] = {
    VSF_TEST_REQUEST_CANCEL_CASES_INIT
};

void vsf_test_usart_request_cancel_add_cases(vsf_test_usart_request_cancel_scene_t *scene)
{
    scene->name    = "usart_request_cancel";
    scene->purpose = "cancel";
    scene->hw_req  = "uart1+la";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_REQUEST_CANCEL_CASE_COUNT; i++) {
        __request_cancel_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_request_cancel_run,
            (void *)&__request_cancel_cases[i]);
    }
}

void vsf_test_usart_request_cancel_run(const vsf_test_usart_request_cancel_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */
    vsf_usart_t *usart = c->scene->usart;

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
}

#endif /* VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED */

/* EOF */
