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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_test_usart_rx_error.h"

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_ERROR_PAYLOAD
#   define VSF_TEST_RX_ERROR_PAYLOAD          "Hello VSF\r\n"
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
    bool     overflow_err;
} __rx_error_ctx_t;

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_error_handler(void *target_ptr, vsf_usart_t *usart_ptr,
                            vsf_usart_irq_mask_t irq_mask)
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
    if (irq_mask & VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR) {
        ctx->overflow_err = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_run(vsf_test_case_t *tc)
{
    vsf_test_usart_rx_parity_error_params_t *p = tc->arg;
    vsf_test_suite_t *suite = tc->suite;
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    vsf_err_t err = vsf_usart_init((vsf_usart_t *)suite->arg, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable((vsf_usart_t *)suite->arg));

        vsf_usart_irq_enable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_PARITY_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.parity_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_PARITY_ERR);

        VSF_TEST_ASSERT(ctx.parity_err);

        while (fsm_rt_cpl != vsf_usart_disable((vsf_usart_t *)suite->arg));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini((vsf_usart_t *)suite->arg);
}
#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_run(vsf_test_case_t *tc)
{
    vsf_test_usart_rx_frame_error_params_t *p = tc->arg;
    vsf_test_suite_t *suite = tc->suite;
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    vsf_err_t err = vsf_usart_init((vsf_usart_t *)suite->arg, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable((vsf_usart_t *)suite->arg));

        vsf_usart_irq_enable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_FRAME_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.frame_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_FRAME_ERR);

        VSF_TEST_ASSERT(ctx.frame_err);

        while (fsm_rt_cpl != vsf_usart_disable((vsf_usart_t *)suite->arg));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini((vsf_usart_t *)suite->arg);
}
#endif /* VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_break_error_run(vsf_test_case_t *tc)
{
    vsf_test_usart_rx_break_error_params_t *p = tc->arg;
    vsf_test_suite_t *suite = tc->suite;
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false, .break_err = false };

    vsf_err_t err = vsf_usart_init((vsf_usart_t *)suite->arg, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable((vsf_usart_t *)suite->arg));

        vsf_usart_irq_enable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_BREAK_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.break_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_BREAK_ERR);

        VSF_TEST_ASSERT(ctx.break_err);

        while (fsm_rt_cpl != vsf_usart_disable((vsf_usart_t *)suite->arg));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini((vsf_usart_t *)suite->arg);
}
#endif /* VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_overflow_error_run(vsf_test_case_t *tc)
{
    vsf_test_usart_rx_overflow_error_params_t *p = tc->arg;
    vsf_test_suite_t *suite = tc->suite;
    /* Only the OVERFLOW IRQ is enabled (no RX/RX_TIMEOUT), so the FIFO is
     * never drained — host's burst quickly exceeds the 32-byte PL011 FIFO
     * and OVERRUN fires. */
    __rx_error_ctx_t ctx = { 0 };

    vsf_err_t err = vsf_usart_init((vsf_usart_t *)suite->arg, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable((vsf_usart_t *)suite->arg));

        vsf_usart_irq_enable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.overflow_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable((vsf_usart_t *)suite->arg, VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR);

        VSF_TEST_ASSERT(ctx.overflow_err);

        while (fsm_rt_cpl != vsf_usart_disable((vsf_usart_t *)suite->arg));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini((vsf_usart_t *)suite->arg);
}
#endif /* VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED */

#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED */

/* EOF */
