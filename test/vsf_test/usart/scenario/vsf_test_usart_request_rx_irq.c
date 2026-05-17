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

#include "vsf_test_usart_request_rx_irq.h"
#include "hardware/regs/uart.h"
#include "hardware/regs/addressmap.h"

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_usart_request_rx_irq_case_t __request_rx_irq_cases[] = {
    VSF_TEST_REQUEST_RX_IRQ_CASES_INIT
};

typedef struct {
    volatile bool       cpl;
    volatile uint32_t   irq_count;
} __req_rx_ctx_t;
static __req_rx_ctx_t s_req_rx_ctx;

static void __req_rx_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    s_req_rx_ctx.irq_count++;
    if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
        s_req_rx_ctx.cpl = true;
    }
}

void vsf_test_usart_request_rx_irq_add_cases(vsf_test_usart_request_rx_irq_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_REQUEST_RX_IRQ_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][96];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_request_rx_irq_%u purpose=request-rx hw_req=uart1+la+host_send refill=%lu",
            (unsigned)__request_rx_irq_cases[i].idx,
            (unsigned long)__request_rx_irq_cases[i].refill_target);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_request_rx_irq_run,
            __cfg_str_pool[i], (void *)&__request_rx_irq_cases[i]);
        __request_rx_irq_cases[i].scene = scene;
    }
}

void vsf_test_usart_request_rx_irq_run(const vsf_test_usart_request_rx_irq_case_t *c)
{
    vsf_usart_t *usart = c->scene->usart;

    vsf_trace_info("USART:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    uint32_t total = (uint32_t)cap.rxfifo_depth * c->refill_target;
    if (total < 32) { total = 32; }
    static uint8_t buf[256];
    if (total > sizeof(buf)) { total = sizeof(buf); }

    s_req_rx_ctx.cpl       = false;
    s_req_rx_ctx.irq_count = 0;

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_TX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = 115200,
        .isr      = { .handler_fn = __req_rx_isr, .target_ptr = NULL,
                      .prio       = vsf_arch_prio_highest },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Enable PL011 internal loopback (UART1 RX has no host writer). */
    volatile uint32_t *uart1_cr = (volatile uint32_t *)(UART1_BASE + UART_UARTCR_OFFSET);
    *uart1_cr |= UART_UARTCR_LBE_BITS;

    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX_CPL);

    err = vsf_usart_request_rx(usart, buf, total);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Self-supply bytes via underlying USART TX — loopback delivers them
     * to RX, which the fifo2req adapter drains into the request buffer. */
    static uint8_t txbuf[256];
    for (uint32_t i = 0; i < total; i++) { txbuf[i] = (uint8_t)('A' + (i % 26)); }
    uint32_t tx_remaining = total;
    uint8_t *tx_src = txbuf;

    /* Host script sends `total` bytes during this window. */
    uint32_t timeout_ms = (total * 10000 / 115200) + 2000;
    uint32_t waited = 0;
    while (!s_req_rx_ctx.cpl && waited < timeout_ms) {
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
    VSF_TEST_ASSERT(s_req_rx_ctx.cpl);
    int_fast32_t cnt = vsf_usart_get_rx_count(usart);
    VSF_TEST_ASSERT(cnt == (int_fast32_t)total);
    vsf_trace_info("USART:REQ_RX_IRQ:irq=%lu count=%ld" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)s_req_rx_ctx.irq_count, (long)cnt);
}

#endif /* VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED */

/* EOF */
