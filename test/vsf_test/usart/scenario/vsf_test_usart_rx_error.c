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

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"
#include "../vsf_test_usart.h"
#include "vsf_test_usart_rx_error.h"

static vsf_test_usart_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED

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
#ifndef VSF_TEST_RX_ERROR_COMMON_BAUDRATE
#   define VSF_TEST_RX_ERROR_COMMON_BAUDRATE  115200
#endif
#ifndef VSF_TEST_RX_ERROR_PRIO
// Must preempt PendSV — see note in vsf_test_usart_rx_irq.c.
#   define VSF_TEST_RX_ERROR_PRIO             vsf_arch_prio_1
#endif

/*============================ TYPES =========================================*/

typedef struct __rx_error_ctx_t {
    bool     parity_err;
    bool     frame_err;
} __rx_error_ctx_t;

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
static const vsf_test_usart_rx_parity_error_case_t __rx_parity_error_cases[] = {
    VSF_TEST_RX_PARITY_ERROR_CASES_INIT
};
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
static const vsf_test_usart_rx_frame_error_case_t __rx_frame_error_cases[] = {
    VSF_TEST_RX_FRAME_ERROR_CASES_INIT
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
}

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_add_cases(vsf_usart_t *usart_instance)
{
    s_scenario.usart_instance = usart_instance;
    for (uint8_t i = 0; i < VSF_TEST_RX_PARITY_ERROR_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][64];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_rx_parity_%u purpose=rx-parity hw_req=uart1+la",
            (unsigned)__rx_parity_error_cases[i].idx);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_rx_parity_error_run,
            __cfg_str_pool[i], (void *)&__rx_parity_error_cases[i]);
    }
}

void vsf_test_usart_rx_parity_error_run(const vsf_test_usart_rx_parity_error_case_t *c)
{
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    vsf_trace_info("RX_PARITY:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(c->scenario->usart_instance, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = VSF_TEST_RX_ERROR_COMMON_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->scenario->usart_instance));

        vsf_usart_irq_enable(c->scenario->usart_instance, VSF_USART_IRQ_MASK_PARITY_ERR);

        vsf_trace_info("RX_PARITY:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.parity_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable(c->scenario->usart_instance, VSF_USART_IRQ_MASK_PARITY_ERR);

        VSF_TEST_ASSERT(ctx.parity_err);

        while (fsm_rt_cpl != vsf_usart_disable(c->scenario->usart_instance));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}
#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_add_cases(vsf_usart_t *usart_instance)
{
    s_scenario.usart_instance = usart_instance;
    for (uint8_t i = 0; i < VSF_TEST_RX_FRAME_ERROR_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][64];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_rx_frame_%u purpose=rx-frame hw_req=uart1+la",
            (unsigned)__rx_frame_error_cases[i].idx);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_rx_frame_error_run,
            __cfg_str_pool[i], (void *)&__rx_frame_error_cases[i]);
    }
}

void vsf_test_usart_rx_frame_error_run(const vsf_test_usart_rx_frame_error_case_t *c)
{
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    vsf_trace_info("RX_FRAME:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(c->scenario->usart_instance, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = VSF_TEST_RX_ERROR_COMMON_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->scenario->usart_instance));

        vsf_usart_irq_enable(c->scenario->usart_instance, VSF_USART_IRQ_MASK_FRAME_ERR);

        vsf_trace_info("RX_FRAME:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.frame_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        vsf_usart_irq_disable(c->scenario->usart_instance, VSF_USART_IRQ_MASK_FRAME_ERR);

        VSF_TEST_ASSERT(ctx.frame_err);

        while (fsm_rt_cpl != vsf_usart_disable(c->scenario->usart_instance));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}
#endif /* VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED */

#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED */

/* EOF */
