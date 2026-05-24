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

#define __VSF_TEST_USART_CLASS_IMPLEMENT
#include "vsf_test_usart_rx_fifo_irq.h"

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED


static vsf_test_usart_rx_fifo_irq_case_t __rx_fifo_irq_cases[] = {
    VSF_TEST_USART_RX_FIFO_IRQ_CASES_INIT
};

static void __rx_fifo_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }
    vsf_test_usart_rx_fifo_irq_suite_t *suite = (vsf_test_usart_rx_fifo_irq_suite_t *)target;
    suite->isr_count++;
    while (suite->received < suite->target) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
        if (avail == 0) { break; }
        uint_fast16_t want = suite->target - suite->received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(usart, suite->dst + suite->received, want);
        suite->received += got;
        if (got == 0) { break; }
    }
    if (suite->received >= suite->target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        suite->done = true;
    }
}

void vsf_test_usart_rx_fifo_irq_add_cases(vsf_test_usart_rx_fifo_irq_suite_t *suite)
{
    suite->name    = "usart_rx_fifo_irq";
    suite->purpose = "rx-fifo-irq";
    suite->hw_req  = "uart1+la+host_send";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_USART_RX_FIFO_IRQ_CASE_COUNT; i++) {
        __rx_fifo_irq_cases[i].suite = suite;
        vsf_test_suite_add_case_ex(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_fifo_irq_run,
            (void *)&__rx_fifo_irq_cases[i], true);
    }
}

void vsf_test_usart_rx_fifo_irq_run(const vsf_test_usart_rx_fifo_irq_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */
    vsf_usart_t *usart = c->suite->usart;

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    VSF_TEST_ASSERT(cap.rxfifo_depth > 0);
    uint32_t total = (uint32_t)cap.rxfifo_depth * c->refill_target;
    static uint8_t buf[256];
    if (total > sizeof(buf)) { total = sizeof(buf); }

    /* Per-case state in suite: must be re-initialised before each run. */
    c->suite->dst       = buf;
    c->suite->received  = 0;
    c->suite->target    = total;
    c->suite->isr_count = 0;
    c->suite->done      = false;

    /* Enable threshold IRQ at the requested level (no timeout) — distinguishes
     * from rx_irq and exercises NOT_EMPTY / HALF_FULL / FULL across cases. */
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_TX_ENABLE
                  | c->threshold_mode,
        .baudrate = 115200,
        .isr      = { .handler_fn = __rx_fifo_isr, .target_ptr = c->suite,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Host sends data via aux_serial after READY marker. RX IRQ fires
     * as bytes arrive. */
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait for host data. Fixed iteration bound — immune to CI jitter. */
    #define RX_FIFO_IRQ_POLL_MAX_ITER 8000   /* ~8 s equivalent with 1 ms step */
    for (uint32_t iter = 0; iter < RX_FIFO_IRQ_POLL_MAX_ITER && !c->suite->done; iter++) {
        vsf_test_busy_wait_ms(1);
    }
    VSF_TEST_ASSERT(c->suite->done);
    VSF_TEST_ASSERT(c->suite->isr_count > 0);
    vsf_trace_info("USART:RX_FIFO_IRQ:isr=%lu got=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->suite->isr_count, (unsigned long)c->suite->received);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED */

/* EOF */
