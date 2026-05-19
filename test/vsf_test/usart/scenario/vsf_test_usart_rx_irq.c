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

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_IRQ_PAYLOAD
#   define VSF_TEST_RX_IRQ_PAYLOAD          "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
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

/*============================ TYPES =========================================*/

typedef struct __rx_irq_ctx_t {
    uint8_t  buf[32];
    uint16_t count;
    uint16_t expected_len;
    bool     done;
} __rx_irq_ctx_t;

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_usart_rx_irq_case_t __rx_irq_cases[] = {
    VSF_TEST_RX_IRQ_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

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

void vsf_test_usart_rx_irq_add_cases(vsf_test_usart_rx_irq_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_RX_IRQ_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][64];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_rx_irq_%u purpose=rx-irq hw_req=uart1+la",
            (unsigned)__rx_irq_cases[i].idx);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_rx_irq_run,
            __cfg_str_pool[i], (void *)&__rx_irq_cases[i]);
        __rx_irq_cases[i].scene = scene;
    }
}

void vsf_test_usart_rx_irq_run(const vsf_test_usart_rx_irq_case_t *c)
{
    __rx_irq_ctx_t ctx = { .count = 0, .expected_len = strlen(VSF_TEST_RX_IRQ_PAYLOAD), .done = false };

    vsf_trace_info("RX_IRQ:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(c->scene->usart, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_RX_IRQ_DEFAULT_MODE,
        .baudrate = VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_irq_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_IRQ_PRIO,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->scene->usart));

        // Drain residual bytes left in the RX FIFO by prior scenarios (e.g.
        // rx_frame_error / rx_parity_error inject framing errors that can
        // leave garbage in the FIFO across scene boundaries). Without this,
        // the ISR fires immediately on enable and pollutes ctx.buf.
        {
            uint8_t junk[16];
            while (vsf_usart_rxfifo_get_data_count(c->scene->usart) > 0) {
                if (vsf_usart_rxfifo_read(c->scene->usart, junk, sizeof(junk)) == 0) break;
            }
        }

        vsf_usart_irq_enable(c->scene->usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

        vsf_trace_info("RX_IRQ:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.done && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable(c->scene->usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.count == ctx.expected_len);
        VSF_TEST_ASSERT(memcmp(ctx.buf, VSF_TEST_RX_IRQ_PAYLOAD, ctx.expected_len) == 0);

        while (fsm_rt_cpl != vsf_usart_disable(c->scene->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }

    vsf_trace_info("RX_IRQ:CASE:%d:DONE" VSF_TRACE_CFG_LINEEND, (int)c->idx);
}

#endif /* VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED */

/* EOF */
