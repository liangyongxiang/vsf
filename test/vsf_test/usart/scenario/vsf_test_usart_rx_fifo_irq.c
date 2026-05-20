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

#include "vsf_test_usart_rx_fifo_irq.h"
#include "hardware/regs/uart.h"
#include "hardware/regs/addressmap.h"

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_usart_rx_fifo_irq_case_t __rx_fifo_irq_cases[] = {
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

void vsf_test_usart_rx_fifo_irq_add_cases(vsf_test_usart_rx_fifo_irq_scene_t *scene)
{
    scene->name    = "usart_rx_fifo_irq";
    scene->purpose = "rx-fifo-irq";
    scene->hw_req  = "uart1+la+host_send";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_RX_FIFO_IRQ_CASE_COUNT; i++) {
        __rx_fifo_irq_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_fifo_irq_run,
            (void *)&__rx_fifo_irq_cases[i]);
    }
}

void vsf_test_usart_rx_fifo_irq_run(const vsf_test_usart_rx_fifo_irq_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */
    vsf_usart_t *usart = c->scene->usart;

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
                  | VSF_USART_TX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = 115200,
        .isr      = { .handler_fn = __rx_fifo_isr, .target_ptr = NULL,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Enable PL011 internal loopback so firmware-side TX feeds RX without
     * needing a host UART connection. The bench's hardware-map only
     * exposes one host serial port (UART0); UART1 RX has no host writer. */
    volatile uint32_t *uart1_cr = (volatile uint32_t *)(UART1_BASE + UART_UARTCR_OFFSET);
    *uart1_cr |= UART_UARTCR_LBE_BITS;

    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Self-supply bytes via TX — loopback feeds them into RX. */
    static uint8_t txbuf[256];
    for (uint32_t i = 0; i < total; i++) { txbuf[i] = (uint8_t)(i & 0xFF); }
    uint32_t tx_remaining = total;
    uint8_t *tx_src = txbuf;

    /* Wait. Loopback supplies bytes via TX → RX as we push them. */
    uint32_t timeout_ms = (total * 10000 / 115200) + 1000;
    uint32_t waited = 0;
    while (!s_rx_ctx.done && waited < timeout_ms) {
        if (tx_remaining > 0) {
            uint_fast16_t want = (tx_remaining > 16) ? 16 : (uint_fast16_t)tx_remaining;
            uint_fast16_t wrote = vsf_usart_txfifo_write(usart, tx_src, want);
            tx_src        += wrote;
            tx_remaining  -= wrote;
        }
        vsf_test_busy_wait_ms(1);
        waited++;
    }
    VSF_TEST_ASSERT(s_rx_ctx.done);
    VSF_TEST_ASSERT(s_rx_ctx.isr_count > 0);
    vsf_trace_info("USART:RX_FIFO_IRQ:isr=%lu got=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)s_rx_ctx.isr_count, (unsigned long)s_rx_ctx.received);

    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED */

/* EOF */
