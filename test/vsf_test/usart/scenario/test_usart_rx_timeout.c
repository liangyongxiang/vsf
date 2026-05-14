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
#include "../test_usart.h"
#include "test_usart_rx_timeout.h"
#include "test_params_generated.h"

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef RX_TIMEOUT_PAYLOAD
#   define RX_TIMEOUT_PAYLOAD          "Hello VSF\r\n"
#endif
#ifndef MARKER_DELAY_MS
#   define MARKER_DELAY_MS             200
#endif
#ifndef RX_TIMEOUT_PAYLOAD_DRAIN_MS
#   define RX_TIMEOUT_PAYLOAD_DRAIN_MS 500
#endif
#ifndef RX_TIMEOUT_COMMON_MODE
#   define RX_TIMEOUT_COMMON_MODE     (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_RX_ENABLE)
#endif
#ifndef RX_TIMEOUT_COMMON_BAUDRATE
#   define RX_TIMEOUT_COMMON_BAUDRATE 115200
#endif

/*============================ TYPES =========================================*/

typedef struct __rx_timeout_ctx_t {
    bool     timeout_triggered;
} __rx_timeout_ctx_t;

/*============================ IMPLEMENTATION ================================*/

static void __busy_wait_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 22000; i++);
}

static void __rx_timeout_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    __rx_timeout_ctx_t *ctx = (__rx_timeout_ctx_t *)target_ptr;

    if (irq_mask & VSF_USART_IRQ_MASK_RX_TIMEOUT) {
        ctx->timeout_triggered = true;
    }
}

/*============================ TEST CASE =====================================*/

void vsf_test_usart_rx_timeout_scenario(void *arg)
{
    const vsf_test_usart_rx_timeout_case_t *c = (const vsf_test_usart_rx_timeout_case_t *)arg;
    __rx_timeout_ctx_t ctx = { .timeout_triggered = false };

    vsf_trace_info("RX:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    __busy_wait_ms(MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(test_usart_instance, &(vsf_usart_cfg_t){
        .mode       = RX_TIMEOUT_COMMON_MODE,
        .baudrate   = RX_TIMEOUT_COMMON_BAUDRATE,
        .rx_timeout = 10000, /* 10ms = 10000us */
        .isr        = {
            .handler_fn = __rx_timeout_handler,
            .target_ptr = &ctx,
            .prio       = vsf_arch_prio_0,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(test_usart_instance));

        vsf_usart_irq_enable(test_usart_instance, VSF_USART_IRQ_MASK_RX_TIMEOUT);

        vsf_trace_info("RX:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t timeout_ticks = vsf_systimer_get_ms() + RX_TIMEOUT_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.timeout_triggered) {
            if (vsf_systimer_get_ms() > timeout_ticks) {
                break;
            }
        }

        vsf_usart_irq_disable(test_usart_instance, VSF_USART_IRQ_MASK_RX_TIMEOUT);

        VSF_TEST_ASSERT(ctx.timeout_triggered);

        while (fsm_rt_cpl != vsf_usart_disable(test_usart_instance));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}

#endif /* VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED */

/* EOF */
