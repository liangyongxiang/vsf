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

#include "vsf_test_usart_rx_fifo_threshold.h"
#include "vsf_test_suites.h"
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE  115200
#endif

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_fifo_threshold_handler(void *target, vsf_usart_t *usart,
                                        vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }

    vsf_test_suite_t *suite = target;

    /* Drain the FIFO completely — PL011 RX interrupt is level-triggered;
     * if we read only 1 byte the level may drop below threshold and the
     * remaining bytes stall because no new data is arriving. */
    while (vsf_usart_rxfifo_get_data_count(usart) > 0) {
        uint_fast16_t want = vsf_test_suites.usart_rx_fifo_threshold.target - vsf_test_suites.usart_rx_fifo_threshold.received;
        if (want == 0) break;
        uint_fast16_t got = vsf_usart_rxfifo_read(
            usart, vsf_test_suites.usart_rx_fifo_threshold.dst + vsf_test_suites.usart_rx_fifo_threshold.received, want);
        if (got == 0) break;
        vsf_test_suites.usart_rx_fifo_threshold.received += got;
    }

    /* Record total bytes received at the first threshold fire.  Because we
     * drain the entire FIFO in one ISR visit, this equals the threshold
     * level (assuming the host sent exactly that many bytes). */
    if (!vsf_test_suites.usart_rx_fifo_threshold.threshold_fired) {
        vsf_test_suites.usart_rx_fifo_threshold.threshold_fired = true;
        vsf_test_suites.usart_rx_fifo_threshold.bytes_at_threshold = vsf_test_suites.usart_rx_fifo_threshold.received;
    }

    vsf_test_suites.usart_rx_fifo_threshold.isr_count++;

    if (vsf_test_suites.usart_rx_fifo_threshold.received >= vsf_test_suites.usart_rx_fifo_threshold.target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        vsf_test_suites.usart_rx_fifo_threshold.done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_fifo_threshold_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_usart_rx_fifo_threshold_params_t *p = tc->arg;
    vsf_usart_t *usart = (vsf_usart_t *)fixture;

    /* Per-case state must be re-initialised before each run. */
    vsf_test_suites.usart_rx_fifo_threshold.dst                = vsf_test_suites.usart_rx_fifo_threshold.rx_fifo_threshold_buf;
    vsf_test_suites.usart_rx_fifo_threshold.target             = p->expected_bytes;
    vsf_test_suites.usart_rx_fifo_threshold.received           = 0;
    vsf_test_suites.usart_rx_fifo_threshold.isr_count          = 0;
    vsf_test_suites.usart_rx_fifo_threshold.done               = false;
    vsf_test_suites.usart_rx_fifo_threshold.threshold_fired    = false;
    vsf_test_suites.usart_rx_fifo_threshold.bytes_at_threshold = 0;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | p->threshold_mode,
        .baudrate = VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_fifo_threshold_handler,
            .target_ptr = NULL,
            .prio       = vsf_arch_prio_highest,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Drain any residual bytes from prior scenarios before enabling the
     * RX threshold interrupt; otherwise a stale byte can trigger a
     * spurious immediate fire with bytes_at_threshold == 0. */
    {
        uint8_t junk[16];
        while (vsf_usart_rxfifo_get_data_count(usart) > 0) {
            if (vsf_usart_rxfifo_read(usart, junk, sizeof(junk)) == 0) break;
        }
    }

    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait for ISR to receive everything.
     * 10 bits/byte @ 115200 = ~87 us/byte.  32 bytes ~ 3 ms.
     * 1 s timeout is generous headroom for host-side scheduling. */
    uint32_t elapsed_ms = 0;
    while (!vsf_test_suites.usart_rx_fifo_threshold.done && elapsed_ms < 1000) {
        vsf_test_busy_wait_ms(1);
        elapsed_ms++;
    }

    VSF_TEST_ASSERT(vsf_test_suites.usart_rx_fifo_threshold.done);
    VSF_TEST_ASSERT(vsf_test_suites.usart_rx_fifo_threshold.received == p->expected_bytes);
    VSF_TEST_ASSERT(vsf_test_suites.usart_rx_fifo_threshold.isr_count > 0);

    /* Core assertion: threshold IRQ fired at exactly the expected byte count. */
    VSF_TEST_ASSERT(vsf_test_suites.usart_rx_fifo_threshold.bytes_at_threshold == p->expected_bytes);

    /* Verify byte-level correctness: incrementing-counter pattern. */
    for (uint32_t i = 0; i < p->expected_bytes; i++) {
        VSF_TEST_ASSERT(vsf_test_suites.usart_rx_fifo_threshold.rx_fifo_threshold_buf[i] == (uint8_t)(i & 0xFF));
    }

    vsf_trace_info("USART:RX_FIFO_THRESHOLD:thr=%s exp=%lu got=%lu isr=%lu" VSF_TRACE_CFG_LINEEND,
                   (p->threshold_mode == VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL) ? "HALF" :
                   (p->threshold_mode == VSF_USART_RX_FIFO_THRESHOLD_FULL) ? "FULL" : "?",
                   (unsigned long)p->expected_bytes,
                   (unsigned long)vsf_test_suites.usart_rx_fifo_threshold.bytes_at_threshold,
                   (unsigned long)vsf_test_suites.usart_rx_fifo_threshold.isr_count);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED */

/* EOF */