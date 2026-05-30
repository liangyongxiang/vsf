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
#include "vsf_test_suites.h"
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE  115200
#endif

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_bulk_irq_handler(void *target, vsf_usart_t *usart,
                                  vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }

    vsf_test_suite_t *suite = target;

    while (vsf_test_suite_data.usart.usart_rx_bulk_irq.received < vsf_test_suite_data.usart.usart_rx_bulk_irq.target) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
        if (avail == 0) { break; }

        uint_fast16_t want = vsf_test_suite_data.usart.usart_rx_bulk_irq.target - vsf_test_suite_data.usart.usart_rx_bulk_irq.received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(
            usart, vsf_test_suite_data.usart.usart_rx_bulk_irq.dst + vsf_test_suite_data.usart.usart_rx_bulk_irq.received, want);
        vsf_test_suite_data.usart.usart_rx_bulk_irq.received += got;
        vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count++;

        if (got < want) { break; }
    }

    if (vsf_test_suite_data.usart.usart_rx_bulk_irq.received >= vsf_test_suite_data.usart.usart_rx_bulk_irq.target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        vsf_test_suite_data.usart.usart_rx_bulk_irq.done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_bulk_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_usart_rx_bulk_irq_params_t *p = tc->arg;
    vsf_usart_t *usart = (vsf_usart_t *)fixture;

    /* Per-case state must be re-initialised before each run. */
    vsf_test_suite_data.usart.usart_rx_bulk_irq.dst       = vsf_test_suite_data.usart.usart_rx_bulk_irq.rx_bulk_irq_buf;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.target    = p->data_size_bytes;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.received  = 0;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count = 0;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.done      = false;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_bulk_irq_handler,
            .target_ptr = NULL,
            .prio       = vsf_arch_prio_highest,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait for ISR to receive everything.  Scale timeout with data size:
     * 10 bits/byte @ 115200 = ~87 µs/byte.  4 KB ≈ 350 ms.
     * Factor of 10 gives comfortable headroom for ISR latency. */
    uint32_t max_ms = (p->data_size_bytes * 10 * 10) / (VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE / 1000);
    if (max_ms < 1000) { max_ms = 1000; }
    uint32_t elapsed_ms = 0;
    while (!vsf_test_suite_data.usart.usart_rx_bulk_irq.done && elapsed_ms < max_ms) {
        vsf_test_busy_wait_ms(10);
        elapsed_ms += 10;
    }

    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.done);
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.received == p->data_size_bytes);
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count > 0);

    /* Verify byte-level correctness: incrementing-counter pattern. */
    for (uint32_t i = 0; i < p->data_size_bytes; i++) {
        VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.rx_bulk_irq_buf[i] == (uint8_t)(i & 0xFF));
    }

    vsf_trace_info("USART:RX_BULK_IRQ:sz=%lu isr=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)p->data_size_bytes,
                   (unsigned long)vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED */

/* EOF */