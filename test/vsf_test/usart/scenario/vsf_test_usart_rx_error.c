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

#include "vsf_test_usart_rx_error.h"

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_ERROR_PAYLOAD
#   define VSF_TEST_RX_ERROR_PAYLOAD          "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS           200
#endif
#ifndef VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS
#   define VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS 500
#endif
#ifndef VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE  115200
#endif
#ifndef VSF_TEST_RX_ERROR_PRIO
// Must preempt PendSV — see note in vsf_test_usart_rx_irq.c.
#   define VSF_TEST_RX_ERROR_PRIO             vsf_arch_prio_1
#endif

/*============================ TYPES =========================================*/

typedef struct __rx_error_ctx_t {
    bool     parity_err;
    bool     frame_err;
    bool     break_err;
} __rx_error_ctx_t;

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
static vsf_test_usart_rx_parity_error_case_t __rx_parity_error_cases[] = {
    VSF_TEST_RX_PARITY_ERROR_CASES_INIT
};
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
static vsf_test_usart_rx_frame_error_case_t __rx_frame_error_cases[] = {
    VSF_TEST_RX_FRAME_ERROR_CASES_INIT
};
#endif

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
static vsf_test_usart_rx_break_error_case_t __rx_break_error_cases[] = {
    VSF_TEST_RX_BREAK_ERROR_CASES_INIT
};
#endif

/*============================ IMPLEMENTATION ================================*/

static void __rx_error_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    __rx_error_ctx_t *ctx = (__rx_error_ctx_t *)target_ptr;

    if (irq_mask & VSF_USART_IRQ_MASK_PARITY_ERR) {
        ctx->parity_err = true;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_FRAME_ERR) {
        ctx->frame_err = true;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_BREAK_ERR) {
        ctx->break_err = true;
    }
}

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_add_cases(vsf_test_usart_rx_parity_error_suite_t *suite)
{
    suite->name    = "usart_rx_parity_error";
    suite->purpose = "rx-parity";
    suite->hw_req  = "uart1+la";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_RX_PARITY_ERROR_CASE_COUNT; i++) {
        __rx_parity_error_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_parity_error_run,
            (void *)&__rx_parity_error_cases[i]);
    }
}

void vsf_test_usart_rx_parity_error_run(const vsf_test_usart_rx_parity_error_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; the per-case ":READY" handshake below is the
     * RX scenario's own marker. */
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    vsf_err_t err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));

        vsf_usart_irq_enable(c->suite->usart, VSF_USART_IRQ_MASK_PARITY_ERR);

        vsf_trace_info("usart_rx_parity_error:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.parity_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable(c->suite->usart, VSF_USART_IRQ_MASK_PARITY_ERR);

        VSF_TEST_ASSERT(ctx.parity_err);

        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->suite->usart);
}
#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_add_cases(vsf_test_usart_rx_frame_error_suite_t *suite)
{
    suite->name    = "usart_rx_frame_error";
    suite->purpose = "rx-frame";
    suite->hw_req  = "uart1+la";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_RX_FRAME_ERROR_CASE_COUNT; i++) {
        __rx_frame_error_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_frame_error_run,
            (void *)&__rx_frame_error_cases[i]);
    }
}

void vsf_test_usart_rx_frame_error_run(const vsf_test_usart_rx_frame_error_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; the per-case ":READY" handshake below is the
     * RX scenario's own marker. */
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    vsf_err_t err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));

        vsf_usart_irq_enable(c->suite->usart, VSF_USART_IRQ_MASK_FRAME_ERR);

        vsf_trace_info("usart_rx_frame_error:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.frame_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable(c->suite->usart, VSF_USART_IRQ_MASK_FRAME_ERR);

        VSF_TEST_ASSERT(ctx.frame_err);

        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->suite->usart);
}
#endif /* VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_break_error_add_cases(vsf_test_usart_rx_break_error_suite_t *suite)
{
    suite->name    = "usart_rx_break_error";
    suite->purpose = "rx-break";
    suite->hw_req  = "uart1+host";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_RX_BREAK_ERROR_CASE_COUNT; i++) {
        __rx_break_error_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_break_error_run,
            (void *)&__rx_break_error_cases[i]);
    }
}

void vsf_test_usart_rx_break_error_run(const vsf_test_usart_rx_break_error_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; the per-case ":READY" handshake below is the
     * RX scenario's own marker. Host responds by toggling tty BRK condition. */
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false, .break_err = false };

    vsf_err_t err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));

        vsf_usart_irq_enable(c->suite->usart, VSF_USART_IRQ_MASK_BREAK_ERR);

        vsf_trace_info("usart_rx_break_error:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.break_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable(c->suite->usart, VSF_USART_IRQ_MASK_BREAK_ERR);

        VSF_TEST_ASSERT(ctx.break_err);

        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->suite->usart);
}
#endif /* VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED */

#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED */

/* EOF */
