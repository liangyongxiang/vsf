/*****************************************************************************
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

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"
#include "../vsf_test_usart.h"
#include "vsf_test_usart_rx_irq.h"
#include "test_params_generated.h"

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
#ifndef VSF_TEST_RX_IRQ_COMMON_MODE
#   define VSF_TEST_RX_IRQ_COMMON_MODE     (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_RX_ENABLE)
#endif
#ifndef VSF_TEST_RX_IRQ_COMMON_BAUDRATE
#   define VSF_TEST_RX_IRQ_COMMON_BAUDRATE 115200
#endif

/*============================ TYPES =========================================*/

typedef struct __rx_irq_ctx_t {
    uint8_t  buf[32];
    uint16_t count;
    uint16_t expected_len;
    bool     done;
} __rx_irq_ctx_t;

/*============================ IMPLEMENTATION ================================*/

static void __busy_wait_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 22000; i++);
}

static void __rx_irq_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    __rx_irq_ctx_t *ctx = (__rx_irq_ctx_t *)target_ptr;

    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart_ptr);
        while (avail-- > 0 && ctx->count < sizeof(ctx->buf)) {
            vsf_usart_rxfifo_read(usart_ptr, &ctx->buf[ctx->count], 1);
            ctx->count++;
        }
        if (ctx->count >= ctx->expected_len) {
            ctx->done = true;
        }
    }
}

/*============================ TEST CASE =====================================*/

void vsf_test_usart_rx_irq_scenario(const vsf_test_usart_rx_irq_case_t *c)
{
    __rx_irq_ctx_t ctx = { .count = 0, .expected_len = strlen(VSF_TEST_RX_IRQ_PAYLOAD), .done = false };

    vsf_trace_info("RX_IRQ:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    __busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(test_usart_rx_instance, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_RX_IRQ_COMMON_MODE,
        .baudrate = VSF_TEST_RX_IRQ_COMMON_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_irq_handler,
            .target_ptr = &ctx,
            .prio       = vsf_arch_prio_0,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(test_usart_rx_instance));

        vsf_usart_irq_enable(test_usart_rx_instance, VSF_USART_IRQ_MASK_RX);

        vsf_trace_info("RX_IRQ:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t timeout_ticks = vsf_systimer_get_ms() + VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.done) {
            if (vsf_systimer_get_ms() > timeout_ticks) {
                break;
            }
        }

        vsf_usart_irq_disable(test_usart_rx_instance, VSF_USART_IRQ_MASK_RX);

        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.count == ctx.expected_len);
        VSF_TEST_ASSERT(memcmp(ctx.buf, VSF_TEST_RX_IRQ_PAYLOAD, ctx.expected_len) == 0);

        while (fsm_rt_cpl != vsf_usart_disable(test_usart_rx_instance));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}

#endif /* VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED */

/* EOF */
