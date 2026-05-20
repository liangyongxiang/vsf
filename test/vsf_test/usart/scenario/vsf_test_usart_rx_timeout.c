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

#include "vsf_test_usart_rx_timeout.h"

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_TIMEOUT_PAYLOAD
#   define VSF_TEST_RX_TIMEOUT_PAYLOAD          "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS             200
#endif
#ifndef VSF_TEST_RX_TIMEOUT_PAYLOAD_DRAIN_MS
#   define VSF_TEST_RX_TIMEOUT_PAYLOAD_DRAIN_MS 500
#endif
#ifndef VSF_TEST_RX_TIMEOUT_DEFAULT_MODE
#   define VSF_TEST_RX_TIMEOUT_DEFAULT_MODE     (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_RX_ENABLE)
#endif
#ifndef VSF_TEST_RX_TIMEOUT_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_TIMEOUT_DEFAULT_BAUDRATE 115200
#endif
#ifndef VSF_TEST_RX_TIMEOUT_PRIO
// Must preempt PendSV — see note in vsf_test_usart_rx_irq.c.
#   define VSF_TEST_RX_TIMEOUT_PRIO        vsf_arch_prio_1
#endif
#ifndef VSF_TEST_RX_TIMEOUT_US
// PL011 RX-idle timeout in microseconds.
#   define VSF_TEST_RX_TIMEOUT_US          10000
#endif

/*============================ TYPES =========================================*/

typedef struct __rx_timeout_ctx_t {
    bool     timeout_triggered;
} __rx_timeout_ctx_t;

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_usart_rx_timeout_case_t __rx_timeout_cases[] = {
    VSF_TEST_RX_TIMEOUT_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

static void __rx_timeout_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    __rx_timeout_ctx_t *ctx = (__rx_timeout_ctx_t *)target_ptr;

    if (irq_mask & VSF_USART_IRQ_MASK_RX_TIMEOUT) {
        ctx->timeout_triggered = true;
    }
}

void vsf_test_usart_rx_timeout_add_cases(vsf_test_usart_rx_timeout_scene_t *scene)
{
    scene->name    = "usart_rx_timeout";
    scene->purpose = "rx-timeout";
    scene->hw_req  = "uart1+la";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_RX_TIMEOUT_CASE_COUNT; i++) {
        __rx_timeout_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_timeout_run,
            (void *)&__rx_timeout_cases[i]);
    }
}

void vsf_test_usart_rx_timeout_run(const vsf_test_usart_rx_timeout_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; the per-case ":READY" handshake below is the
     * RX scenario's own marker. */
    __rx_timeout_ctx_t ctx = { .timeout_triggered = false };

    vsf_err_t err = vsf_usart_init(c->scene->usart, &(vsf_usart_cfg_t){
        .mode       = VSF_TEST_RX_TIMEOUT_DEFAULT_MODE,
        .baudrate   = VSF_TEST_RX_TIMEOUT_DEFAULT_BAUDRATE,
        .rx_timeout = VSF_TEST_RX_TIMEOUT_US,
        .isr        = {
            .handler_fn = __rx_timeout_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_TIMEOUT_PRIO,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->scene->usart));

        vsf_usart_irq_enable(c->scene->usart, VSF_USART_IRQ_MASK_RX_TIMEOUT);

        vsf_trace_info("usart_rx_timeout:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_TIMEOUT_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.timeout_triggered && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable(c->scene->usart, VSF_USART_IRQ_MASK_RX_TIMEOUT);

        VSF_TEST_ASSERT(ctx.timeout_triggered);

        while (fsm_rt_cpl != vsf_usart_disable(c->scene->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->scene->usart);
}

#endif /* VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED */

/* EOF */
