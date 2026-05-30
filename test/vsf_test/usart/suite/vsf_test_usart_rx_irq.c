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

#include "vsf_test_usart_rx_irq.h"
/*============================ LOCAL VARIABLES ===============================*/

typedef struct __rx_irq_ctx_t {
    uint8_t  buf[32];
    uint16_t count;
    uint16_t expected_len;
    bool     done;
} __rx_irq_ctx_t;

static void __rx_irq_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    __rx_irq_ctx_t *ctx = (__rx_irq_ctx_t *)target_ptr;

    if (irq_mask & (VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT)) {
        while (vsf_usart_rxfifo_get_data_count(usart_ptr) > 0 && ctx->count < sizeof(ctx->buf)) {
            uint_fast16_t read = vsf_usart_rxfifo_read(usart_ptr, &ctx->buf[ctx->count], sizeof(ctx->buf) - ctx->count);
            if (read == 0) break;
            ctx->count += read;
        }
        if (ctx->count >= ctx->expected_len) {
            ctx->done = true;
        }
    }
}



#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_IRQ_PAYLOAD
#   define VSF_TEST_RX_IRQ_PAYLOAD          "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS
#   define VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS 500
#endif
#ifndef VSF_TEST_RX_IRQ_DEFAULT_MODE
#   define VSF_TEST_RX_IRQ_DEFAULT_MODE     (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_RX_ENABLE)
#endif
#ifndef VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE 115200
#endif
#ifndef VSF_TEST_RX_IRQ_PRIO
// Must be higher (numerically lower) than PendSV priority — test framework runs
// scenarios inside PendSV, so a same-priority IRQ cannot preempt and the handler
// would never run.
#   define VSF_TEST_RX_IRQ_PRIO            vsf_arch_prio_1
#endif

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_usart_rx_irq_params_t *p = tc->arg;
    __rx_irq_ctx_t ctx = { .count = 0, .expected_len = strlen(VSF_TEST_RX_IRQ_PAYLOAD), .done = false };

    vsf_err_t err = vsf_usart_init((vsf_usart_t *)fixture, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_RX_IRQ_DEFAULT_MODE,
        .baudrate = VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_irq_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_IRQ_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable((vsf_usart_t *)fixture));

        // Drain residual bytes left in the RX FIFO by prior scenarios (e.g.
        // rx_frame_error / rx_parity_error inject framing errors that can
        // leave garbage in the FIFO across suite boundaries). Without this,
        // the ISR fires immediately on enable and pollutes ctx.buf.
        {
            uint8_t junk[VSF_TEST_USART_RX_IRQ_JUNK_SIZE];
            while (vsf_usart_rxfifo_get_data_count((vsf_usart_t *)fixture) > 0) {
                if (vsf_usart_rxfifo_read((vsf_usart_t *)fixture, junk, sizeof(junk)) == 0) break;
            }
        }

        vsf_usart_irq_enable((vsf_usart_t *)fixture, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.done && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable((vsf_usart_t *)fixture, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.count == ctx.expected_len);
        VSF_TEST_ASSERT(memcmp(ctx.buf, VSF_TEST_RX_IRQ_PAYLOAD, ctx.expected_len) == 0);

        while (fsm_rt_cpl != vsf_usart_disable((vsf_usart_t *)fixture));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini((vsf_usart_t *)fixture);
}

#endif /* VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED */

/* EOF */
