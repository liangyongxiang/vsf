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

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"
#include "../vsf_test_usart.h"
#include "vsf_test_usart_rx_fifo_irq.h"

static vsf_test_usart_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_usart_rx_fifo_irq_case_t __rx_fifo_irq_cases[] = {
    VSF_TEST_RX_FIFO_IRQ_CASES_INIT
};

typedef struct {
    vsf_usart_t        *usart;
    uint8_t            *dst;
    uint32_t            received;
    uint32_t            target;
    volatile uint32_t   isr_count;
    volatile bool       done;
} __rx_fifo_ctx_t;
static __rx_fifo_ctx_t s_rx_ctx;

static void __rx_fifo_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }
    s_rx_ctx.isr_count++;
    while (s_rx_ctx.received < s_rx_ctx.target) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
        if (avail == 0) { break; }
        uint_fast16_t want = s_rx_ctx.target - s_rx_ctx.received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(usart, s_rx_ctx.dst + s_rx_ctx.received, want);
        s_rx_ctx.received += got;
        if (got == 0) { break; }
    }
    if (s_rx_ctx.received >= s_rx_ctx.target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        s_rx_ctx.done = true;
    }
}

void vsf_test_usart_rx_fifo_irq_add_cases(vsf_usart_t *usart_instance)
{
    s_scenario.usart_instance = usart_instance;
    for (uint8_t i = 0; i < VSF_TEST_RX_FIFO_IRQ_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][96];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_rx_fifo_irq_%u purpose=rx-fifo-irq hw_req=uart1+la+host_send refill=%lu",
            (unsigned)__rx_fifo_irq_cases[i].idx,
            (unsigned long)__rx_fifo_irq_cases[i].refill_target);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_rx_fifo_irq_run,
            __cfg_str_pool[i], (void *)&__rx_fifo_irq_cases[i]);
    }
}

void vsf_test_usart_rx_fifo_irq_run(const vsf_test_usart_rx_fifo_irq_case_t *c)
{
    vsf_usart_t *usart = c->scenario->usart_instance;

    vsf_trace_info("USART:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    VSF_TEST_ASSERT(cap.rxfifo_depth > 0);
    uint32_t total = (uint32_t)cap.rxfifo_depth * c->refill_target;
    static uint8_t buf[256];
    if (total > sizeof(buf)) { total = sizeof(buf); }

    s_rx_ctx.usart     = usart;
    s_rx_ctx.dst       = buf;
    s_rx_ctx.received  = 0;
    s_rx_ctx.target    = total;
    s_rx_ctx.isr_count = 0;
    s_rx_ctx.done      = false;

    /* Enable ONLY threshold IRQ (no timeout) — distinguishes from rx_irq. */
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = 115200,
        .isr      = { .handler_fn = __rx_fifo_isr, .target_ptr = NULL,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait. Host script must send `total` bytes during this window. */
    uint32_t timeout_ms = (total * 10000 / 115200) + 1000;
    uint32_t waited = 0;
    while (!s_rx_ctx.done && waited < timeout_ms) {
        vsf_test_busy_wait_ms(1);
        waited++;
    }
    VSF_TEST_ASSERT(s_rx_ctx.done);
    VSF_TEST_ASSERT(s_rx_ctx.isr_count > 0);
    vsf_trace_info("USART:RX_FIFO_IRQ:isr=%lu got=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)s_rx_ctx.isr_count, (unsigned long)s_rx_ctx.received);
}

#endif /* VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED */

/* EOF */
