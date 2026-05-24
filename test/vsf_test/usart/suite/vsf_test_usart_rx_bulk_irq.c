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

#include "vsf_test_usart_rx_bulk_irq.h"

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE  115200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static uint8_t __rx_bulk_irq_buf[4096];

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_bulk_irq_handler(void *target, vsf_usart_t *usart,
                                  vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }

    vsf_test_usart_rx_bulk_irq_suite_t *suite =
        (vsf_test_usart_rx_bulk_irq_suite_t *)target;

    while (suite->received < suite->target) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
        if (avail == 0) { break; }

        uint_fast16_t want = suite->target - suite->received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(
            usart, suite->dst + suite->received, want);
        suite->received += got;
        suite->isr_count++;

        if (got < want) { break; }
    }

    if (suite->received >= suite->target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        suite->done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

VSF_TEST_SUITE_REGISTER(vsf_test_usart_rx_bulk_irq_add_cases,
    vsf_test_usart_rx_bulk_irq_suite_t,
    vsf_test_usart_rx_bulk_irq_case_t,
    vsf_test_usart_rx_bulk_irq_run,
    VSF_TEST_USART_RX_BULK_IRQ_CASES_INIT,
    "usart_rx_bulk_irq", "rx-bulk-irq", "uart1+host",
    true)

void vsf_test_usart_rx_bulk_irq_run(const vsf_test_usart_rx_bulk_irq_case_t *c)
{
    vsf_usart_t *usart = c->suite->usart;

    /* Per-case state must be re-initialised before each run. */
    c->suite->dst       = __rx_bulk_irq_buf;
    c->suite->target    = c->data_size_bytes;
    c->suite->received  = 0;
    c->suite->isr_count = 0;
    c->suite->done      = false;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_bulk_irq_handler,
            .target_ptr = c->suite,
            .prio       = vsf_arch_prio_highest,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait for ISR to receive everything.  Scale timeout with data size:
     * 10 bits/byte @ 115200 = ~87 µs/byte.  4 KB ≈ 350 ms.
     * Factor of 10 gives comfortable headroom for ISR latency. */
    uint32_t max_ms = (c->data_size_bytes * 10 * 10) / (VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE / 1000);
    if (max_ms < 1000) { max_ms = 1000; }
    uint32_t elapsed_ms = 0;
    while (!c->suite->done && elapsed_ms < max_ms) {
        vsf_test_busy_wait_ms(10);
        elapsed_ms += 10;
    }

    VSF_TEST_ASSERT(c->suite->done);
    VSF_TEST_ASSERT(c->suite->received == c->data_size_bytes);
    VSF_TEST_ASSERT(c->suite->isr_count > 0);

    /* Verify byte-level correctness: incrementing-counter pattern. */
    for (uint32_t i = 0; i < c->data_size_bytes; i++) {
        VSF_TEST_ASSERT(__rx_bulk_irq_buf[i] == (uint8_t)(i & 0xFF));
    }

    vsf_trace_info("USART:RX_BULK_IRQ:sz=%lu isr=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->data_size_bytes,
                   (unsigned long)c->suite->isr_count);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED */

/* EOF */
