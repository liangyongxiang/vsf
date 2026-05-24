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

#include "vsf_test_usart_rx_fifo_threshold.h"

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE  115200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_usart_rx_fifo_threshold_case_t __rx_fifo_threshold_cases[] = {
    VSF_TEST_USART_RX_FIFO_THRESHOLD_CASES_INIT
};

static uint8_t __rx_fifo_threshold_buf[64];

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_fifo_threshold_handler(void *target, vsf_usart_t *usart,
                                        vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }

    vsf_test_usart_rx_fifo_threshold_suite_t *suite =
        (vsf_test_usart_rx_fifo_threshold_suite_t *)target;

    /* Read all available bytes when threshold IRQ fires. */
    uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
    if (avail > 0) {
        uint_fast16_t want = suite->target - suite->received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(
            usart, suite->dst + suite->received, want);
        suite->received += got;
    }

    suite->isr_count++;

    /* Record the byte count at first threshold IRQ fire. */
    if (!suite->threshold_fired) {
        suite->threshold_fired = true;
        suite->bytes_at_threshold = suite->received;
    }

    if (suite->received >= suite->target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        suite->done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_fifo_threshold_add_cases(vsf_test_usart_rx_fifo_threshold_suite_t *suite)
{
    suite->name    = "usart_rx_fifo_threshold";
    suite->purpose = "rx-fifo-threshold";
    suite->hw_req  = "uart1+host";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_USART_RX_FIFO_THRESHOLD_CASE_COUNT; i++) {
        __rx_fifo_threshold_cases[i].suite = suite;
        vsf_test_suite_add_case_ex(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_fifo_threshold_run,
            (void *)&__rx_fifo_threshold_cases[i], true);
    }
}

void vsf_test_usart_rx_fifo_threshold_run(const vsf_test_usart_rx_fifo_threshold_case_t *c)
{
    vsf_usart_t *usart = c->suite->usart;

    /* Per-case state must be re-initialised before each run. */
    c->suite->dst                = __rx_fifo_threshold_buf;
    c->suite->target             = c->expected_bytes;
    c->suite->received           = 0;
    c->suite->isr_count          = 0;
    c->suite->done               = false;
    c->suite->threshold_fired    = false;
    c->suite->bytes_at_threshold = 0;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | c->threshold_mode,
        .baudrate = VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_fifo_threshold_handler,
            .target_ptr = c->suite,
            .prio       = vsf_arch_prio_highest,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait for ISR to receive everything.
     * 10 bits/byte @ 115200 = ~87 us/byte.  32 bytes ~ 3 ms.
     * 1 s timeout is generous headroom for host-side scheduling. */
    uint32_t elapsed_ms = 0;
    while (!c->suite->done && elapsed_ms < 1000) {
        vsf_test_busy_wait_ms(1);
        elapsed_ms++;
    }

    VSF_TEST_ASSERT(c->suite->done);
    VSF_TEST_ASSERT(c->suite->received == c->expected_bytes);
    VSF_TEST_ASSERT(c->suite->isr_count > 0);

    /* Core assertion: threshold IRQ fired at exactly the expected byte count. */
    VSF_TEST_ASSERT(c->suite->bytes_at_threshold == c->expected_bytes);

    /* Verify byte-level correctness: incrementing-counter pattern. */
    for (uint32_t i = 0; i < c->expected_bytes; i++) {
        VSF_TEST_ASSERT(__rx_fifo_threshold_buf[i] == (uint8_t)(i & 0xFF));
    }

    vsf_trace_info("USART:RX_FIFO_THRESHOLD:thr=%s exp=%lu got=%lu isr=%lu" VSF_TRACE_CFG_LINEEND,
                   (c->threshold_mode == VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL) ? "HALF" :
                   (c->threshold_mode == VSF_USART_RX_FIFO_THRESHOLD_FULL) ? "FULL" : "?",
                   (unsigned long)c->expected_bytes,
                   (unsigned long)c->suite->bytes_at_threshold,
                   (unsigned long)c->suite->isr_count);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED */

/* EOF */
