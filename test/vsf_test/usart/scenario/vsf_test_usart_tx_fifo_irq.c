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
#include "vsf_test_usart_tx_fifo_irq.h"

static vsf_test_usart_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_usart_tx_fifo_irq_case_t __tx_fifo_irq_cases[] = {
    VSF_TEST_TX_FIFO_IRQ_CASES_INIT
};

/* Shared ISR state. Single instance at a time. */
typedef struct {
    vsf_usart_t        *usart;
    const uint8_t      *src;
    uint32_t            remaining;
    volatile uint32_t   isr_count;
    volatile bool       done;
} __tx_fifo_ctx_t;
static __tx_fifo_ctx_t s_tx_ctx;

static void __tx_fifo_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_TX)) { return; }
    s_tx_ctx.isr_count++;
    /* Refill in one large request — txfifo_write reports the actual count
     * written when the FIFO fills, even on PL011 where get_free_count
     * returns only 0/1. */
    while (s_tx_ctx.remaining > 0) {
        uint_fast16_t want = (s_tx_ctx.remaining > 64) ? 64 : (uint_fast16_t)s_tx_ctx.remaining;
        uint_fast16_t wrote = vsf_usart_txfifo_write(usart, (void *)s_tx_ctx.src, want);
        s_tx_ctx.src       += wrote;
        s_tx_ctx.remaining -= wrote;
        if (wrote < want) { break; }  /* FIFO full */
    }
    if (s_tx_ctx.remaining == 0) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_TX);
        s_tx_ctx.done = true;
    }
}

void vsf_test_usart_tx_fifo_irq_add_cases(vsf_usart_t *usart_instance)
{
    s_scenario.usart_instance = usart_instance;
    for (uint8_t i = 0; i < VSF_TEST_TX_FIFO_IRQ_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_tx_fifo_irq_%u purpose=tx-fifo-irq hw_req=uart1+la refill=%lu",
            (unsigned)__tx_fifo_irq_cases[i].idx,
            (unsigned long)__tx_fifo_irq_cases[i].refill_target);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_tx_fifo_irq_run,
            __cfg_str_pool[i], (void *)&__tx_fifo_irq_cases[i]);
    }
}

void vsf_test_usart_tx_fifo_irq_run(const vsf_test_usart_tx_fifo_irq_case_t *c)
{
    vsf_usart_t *usart = c->scenario->usart_instance;

    vsf_trace_info("USART:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    VSF_TEST_ASSERT(cap.txfifo_depth > 0);
    uint32_t total = (uint32_t)cap.txfifo_depth * c->refill_target;
    static uint8_t buf[256];
    if (total > sizeof(buf)) { total = sizeof(buf); }
    for (uint32_t i = 0; i < total; i++) { buf[i] = (uint8_t)('A' + (i % 26)); }

    s_tx_ctx.usart     = usart;
    s_tx_ctx.src       = buf;
    s_tx_ctx.remaining = total;
    s_tx_ctx.isr_count = 0;
    s_tx_ctx.done      = false;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE
                  | VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY,
        .baudrate = 115200,
        .isr      = { .handler_fn = __tx_fifo_isr, .target_ptr = NULL,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Pre-fill the FIFO until it's full (above HALF_EMPTY threshold).
     * txfifo_get_free_count returns only 0 or 1 on PL011 (UARTFR has empty/
     * full flags, not a counter — see ADR-0004), so we can't ask the driver
     * how many slots are free. Instead request `txfifo_depth` bytes and rely
     * on txfifo_write returning the partial count when the FIFO fills. */
    uint_fast16_t prefill_request = cap.txfifo_depth;
    if (prefill_request > s_tx_ctx.remaining) {
        prefill_request = (uint_fast16_t)s_tx_ctx.remaining;
    }
    uint_fast16_t wrote = vsf_usart_txfifo_write(usart, (void *)s_tx_ctx.src, prefill_request);
    s_tx_ctx.src       += wrote;
    s_tx_ctx.remaining -= wrote;
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX);

    /* Wait for ISR to drain everything. Bound the wait to avoid hang. */
    uint32_t timeout_ms = (total * 10000 / 115200) + 500;   /* bit-time + slack */
    uint32_t waited = 0;
    while (!s_tx_ctx.done && waited < timeout_ms) {
        vsf_test_busy_wait_ms(1);
        waited++;
    }
    VSF_TEST_ASSERT(s_tx_ctx.done);
    VSF_TEST_ASSERT(s_tx_ctx.isr_count > 0);
    vsf_trace_info("USART:TX_FIFO_IRQ:isr=%lu total=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)s_tx_ctx.isr_count, (unsigned long)total);
}

#endif /* VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED */

/* EOF */
