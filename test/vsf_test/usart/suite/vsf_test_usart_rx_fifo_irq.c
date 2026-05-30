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

#define __VSF_TEST_USART_CLASS_IMPLEMENT
#include "vsf_test_usart_rx_fifo_irq.h"
#include "vsf_test_suites.h"

/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_FIFO_IRQ_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_FIFO_IRQ_DEFAULT_BAUDRATE  115200
#endif

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_fifo_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & (VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT))) { return; }
    vsf_test_suite_t *suite = target;
    vsf_test_suite_data.usart_rx_fifo_irq.isr_count++;
    while (vsf_test_suite_data.usart_rx_fifo_irq.received < vsf_test_suite_data.usart_rx_fifo_irq.target) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
        if (avail == 0) { break; }
        uint_fast16_t want = vsf_test_suite_data.usart_rx_fifo_irq.target - vsf_test_suite_data.usart_rx_fifo_irq.received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(usart, vsf_test_suite_data.usart_rx_fifo_irq.dst + vsf_test_suite_data.usart_rx_fifo_irq.received, want);
        vsf_test_suite_data.usart_rx_fifo_irq.received += got;
        if (got == 0) { break; }
    }
    if (vsf_test_suite_data.usart_rx_fifo_irq.received >= vsf_test_suite_data.usart_rx_fifo_irq.target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);
        vsf_test_suite_data.usart_rx_fifo_irq.done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_fifo_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_usart_rx_fifo_irq_params_t *p = tc->arg;
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    vsf_usart_t *usart = (vsf_usart_t *)fixture;

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    VSF_TEST_ASSERT(cap.rxfifo_depth > 0);
    uint32_t total = (uint32_t)cap.rxfifo_depth * p->refill_target;
    static uint8_t buf[VSF_TEST_USART_RX_FIFO_IRQ_BUF_SIZE];
    if (total > sizeof(buf)) { total = sizeof(buf); }

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.usart_rx_fifo_irq.dst       = buf;
    vsf_test_suite_data.usart_rx_fifo_irq.received  = 0;
    vsf_test_suite_data.usart_rx_fifo_irq.target    = total;
    vsf_test_suite_data.usart_rx_fifo_irq.isr_count = 0;
    vsf_test_suite_data.usart_rx_fifo_irq.done      = false;

    /* Enable threshold IRQ at the requested level (no timeout) — distinguishes
     * from rx_irq and exercises NOT_EMPTY / HALF_FULL / FULL across cases. */
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_TX_ENABLE
                  | p->threshold_mode,
        .baudrate = VSF_TEST_RX_FIFO_IRQ_DEFAULT_BAUDRATE,
        .isr      = { .handler_fn = __rx_fifo_isr, .target_ptr = NULL,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Host sends data via aux_serial after READY marker. RX IRQ fires
     * as bytes arrive. */
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

    /* Wait for host data. Fixed iteration bound — immune to CI jitter. */
    #define RX_FIFO_IRQ_POLL_MAX_ITER 8000   /* ~8 s equivalent with 1 ms step */
    for (uint32_t iter = 0; iter < RX_FIFO_IRQ_POLL_MAX_ITER && !vsf_test_suite_data.usart_rx_fifo_irq.done; iter++) {
        vsf_test_busy_wait_ms(1);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart_rx_fifo_irq.done);
    VSF_TEST_ASSERT(vsf_test_suite_data.usart_rx_fifo_irq.isr_count > 0);
    vsf_trace_info("USART:RX_FIFO_IRQ:isr=%lu got=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)vsf_test_suite_data.usart_rx_fifo_irq.isr_count, (unsigned long)vsf_test_suite_data.usart_rx_fifo_irq.received);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED */

/* EOF */