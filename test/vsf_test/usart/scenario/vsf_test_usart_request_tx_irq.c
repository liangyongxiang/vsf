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

#include "vsf_test_usart_request_tx_irq.h"

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_usart_request_tx_irq_case_t __request_tx_irq_cases[] = {
    VSF_TEST_REQUEST_TX_IRQ_CASES_INIT
};

typedef struct {
    volatile bool       cpl;
    volatile uint32_t   irq_count;
} __req_tx_ctx_t;
static __req_tx_ctx_t s_req_tx_ctx;

static void __req_tx_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    s_req_tx_ctx.irq_count++;
    if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
        s_req_tx_ctx.cpl = true;
    }
}

void vsf_test_usart_request_tx_irq_add_cases(vsf_test_usart_request_tx_irq_scene_t *scene)
{
    /* The instance passed in must be a fifo2req_usart adapter wrapping the
     * underlying hardware. Application code is responsible for declaring it
     * via describe_fifo2req_usart() and passing the adapter as usart_instance.
     */
    for (uint8_t i = 0; i < VSF_TEST_REQUEST_TX_IRQ_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][96];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_request_tx_irq_%u purpose=request-tx hw_req=uart1+la refill=%lu",
            (unsigned)__request_tx_irq_cases[i].idx,
            (unsigned long)__request_tx_irq_cases[i].refill_target);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_request_tx_irq_run,
            __cfg_str_pool[i], (void *)&__request_tx_irq_cases[i]);
        __request_tx_irq_cases[i].scene = scene;
    }
}

void vsf_test_usart_request_tx_irq_run(const vsf_test_usart_request_tx_irq_case_t *c)
{
    vsf_usart_t *usart = c->scene->usart;

    vsf_trace_info("USART:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    uint32_t total = (uint32_t)cap.txfifo_depth * c->refill_target;
    if (total < 32) { total = 32; }
    static uint8_t buf[256];
    if (total > sizeof(buf)) { total = sizeof(buf); }
    for (uint32_t i = 0; i < total; i++) { buf[i] = (uint8_t)('a' + (i % 26)); }

    s_req_tx_ctx.cpl       = false;
    s_req_tx_ctx.irq_count = 0;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE
                  | VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY,
        .baudrate = 115200,
        .isr      = { .handler_fn = __req_tx_isr, .target_ptr = NULL,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX_CPL);

    err = vsf_usart_request_tx(usart, buf, total);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    uint32_t timeout_ms = (total * 10000 / 115200) + 500;
    uint32_t waited = 0;
    while (!s_req_tx_ctx.cpl && waited < timeout_ms) {
        vsf_test_busy_wait_ms(1);
        waited++;
    }
    VSF_TEST_ASSERT(s_req_tx_ctx.cpl);
    int_fast32_t cnt = vsf_usart_get_tx_count(usart);
    VSF_TEST_ASSERT(cnt == (int_fast32_t)total);
    vsf_trace_info("USART:REQ_TX_IRQ:irq=%lu count=%ld" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)s_req_tx_ctx.irq_count, (long)cnt);
}

#endif /* VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED */

/* EOF */
