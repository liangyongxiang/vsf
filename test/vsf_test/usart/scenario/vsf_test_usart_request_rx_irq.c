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
#include "vsf_test_usart_request_rx_irq.h"
#include "hardware/regs/uart.h"
#include "hardware/regs/addressmap.h"

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED


static vsf_test_usart_request_rx_irq_case_t __request_rx_irq_cases[] = {
    VSF_TEST_REQUEST_RX_IRQ_CASES_INIT
};

static void __req_rx_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    vsf_test_usart_request_rx_irq_suite_t *suite = (vsf_test_usart_request_rx_irq_suite_t *)target;
    suite->req_rx_irq_count++;
    if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
        suite->req_rx_cpl = true;
    }
}

void vsf_test_usart_request_rx_irq_add_cases(vsf_test_usart_request_rx_irq_suite_t *suite)
{
    suite->name    = "usart_request_rx_irq";
    suite->purpose = "request-rx";
    suite->hw_req  = "uart1+la+host_send";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_REQUEST_RX_IRQ_CASE_COUNT; i++) {
        __request_rx_irq_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_request_rx_irq_run,
            (void *)&__request_rx_irq_cases[i]);
    }
}

void vsf_test_usart_request_rx_irq_run(const vsf_test_usart_request_rx_irq_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */
    vsf_usart_t *usart = c->suite->usart;

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    uint32_t total = (uint32_t)cap.rxfifo_depth * c->refill_target;
    if (total < 32) { total = 32; }
    if (total > sizeof(c->suite->req_rx_buf)) { total = sizeof(c->suite->req_rx_buf); }

    /* Per-case state in suite: must be re-initialised before each run. */
    c->suite->req_rx_cpl       = false;
    c->suite->req_rx_irq_count = 0;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_TX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = 115200,
        .isr      = { .handler_fn = __req_rx_isr, .target_ptr = c->suite,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Enable PL011 internal loopback (UART1 RX has no host writer). */
    volatile uint32_t *uart1_cr = (volatile uint32_t *)(UART1_BASE + UART_UARTCR_OFFSET);
    *uart1_cr |= UART_UARTCR_LBE_BITS;

    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX_CPL);

    err = vsf_usart_request_rx(usart, c->suite->req_rx_buf, total);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Self-supply bytes via underlying USART TX — loopback delivers them
     * to RX, which the fifo2req adapter drains into the request buffer. */
    for (uint32_t i = 0; i < total; i++) {
        c->suite->req_rx_txbuf[i] = (uint8_t)('A' + (i % 26));
    }
    uint32_t tx_remaining = total;
    uint8_t *tx_src = c->suite->req_rx_txbuf;

    /* Host script sends `total` bytes during this window. */
    uint32_t timeout_ms = (total * 10000 / 115200) + 2000;
    uint32_t waited = 0;
    while (!c->suite->req_rx_cpl && waited < timeout_ms) {
        if (tx_remaining > 0) {
            /* Write through the underlying hw usart's TX FIFO — bypasses
             * the fifo2req adapter (it owns the RX side via request_rx). */
            extern vsf_hw_usart_t vsf_hw_usart1;
            uint_fast16_t want = (tx_remaining > 16) ? 16 : (uint_fast16_t)tx_remaining;
            uint_fast16_t wrote = vsf_hw_usart_txfifo_write(&vsf_hw_usart1, tx_src, want);
            tx_src       += wrote;
            tx_remaining -= wrote;
        }
        vsf_test_busy_wait_ms(1);
        waited++;
    }
    VSF_TEST_ASSERT(c->suite->req_rx_cpl);
    int_fast32_t cnt = vsf_usart_get_rx_count(usart);
    VSF_TEST_ASSERT(cnt == (int_fast32_t)total);
    vsf_trace_info("USART:REQ_RX_IRQ:irq=%lu count=%ld" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->suite->req_rx_irq_count, (long)cnt);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED */

/* EOF */
