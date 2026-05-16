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
#include "../vsf_test_gpio.h"
#include "vsf_test_gpio_irq_latency.h"

static vsf_test_gpio_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_gpio_irq_latency_case_t __gpio_irq_latency_cases[] = {
    VSF_TEST_GPIO_IRQ_LATENCY_CASES_INIT
};

typedef struct {
    vsf_systimer_tick_t  trigger_tick;
    volatile vsf_systimer_tick_t isr_tick;
    volatile bool        fired;
    vsf_gpio_pin_mask_t  expected_pin;
} __latency_ctx_t;
static __latency_ctx_t s_latency_ctx;

static void __latency_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    if (pin_mask & s_latency_ctx.expected_pin) {
        s_latency_ctx.isr_tick = vsf_systimer_get();
        s_latency_ctx.fired = true;
    }
}

void vsf_test_gpio_irq_latency_add_cases(vsf_gpio_t *gpio_instance)
{
    s_scenario.gpio_instance = gpio_instance;
    for (uint8_t i = 0; i < VSF_TEST_GPIO_IRQ_LATENCY_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_irq_latency_%u purpose=perf-irq pin=%u max_ns=%lu",
            (unsigned)__gpio_irq_latency_cases[i].idx,
            (unsigned)__gpio_irq_latency_cases[i].pin,
            (unsigned long)__gpio_irq_latency_cases[i].max_latency_ns);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_irq_latency_run,
            __cfg_str_pool[i], (void *)&__gpio_irq_latency_cases[i]);
    }
}

void vsf_test_gpio_irq_latency_run(const vsf_test_gpio_irq_latency_case_t *c)
{
    vsf_gpio_t *gpio = c->scenario->gpio_instance;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    s_latency_ctx.expected_pin = pin_mask;

    /* Configure pin as EXTI rising edge — driven by SIO from the same test
     * (self-trigger; no external wiring needed). */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_DOWN | VSF_GPIO_EXTI_MODE_RISING,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __latency_handler,
        .target_ptr = NULL,
        .prio       = vsf_arch_prio_highest,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_gpio_set_output(gpio, pin_mask);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    err = vsf_gpio_exti_irq_enable(gpio, pin_mask);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Repeat the measurement several times and pick the worst case.
     * The first run is often perturbed by cache/prefetch warming. */
    uint32_t worst_ticks = 0;
    const uint32_t ITERATIONS = 8;
    for (uint32_t i = 0; i < ITERATIONS; i++) {
        s_latency_ctx.fired = false;
        s_latency_ctx.isr_tick = 0;
        vsf_gpio_clear(gpio, pin_mask);
        vsf_test_busy_wait_ms(1);
        vsf_gpio_exti_irq_clear(gpio, pin_mask);

        s_latency_ctx.trigger_tick = vsf_systimer_get();
        vsf_gpio_set(gpio, pin_mask);   /* rising edge → EXTI fires */
        /* Spin until ISR captures its tick. */
        for (uint32_t spin = 0; spin < 100000 && !s_latency_ctx.fired; spin++) {
            __asm__ volatile("nop");
        }
        VSF_TEST_ASSERT(s_latency_ctx.fired);
        uint32_t delta = (uint32_t)(s_latency_ctx.isr_tick - s_latency_ctx.trigger_tick);
        if (delta > worst_ticks) { worst_ticks = delta; }
    }

    uint64_t worst_us = vsf_systimer_tick_to_us(worst_ticks);
    uint32_t worst_ns = (uint32_t)(worst_us * 1000ULL);

    vsf_trace_info("GPIO:IRQ_LATENCY:worst_ticks=%lu worst_ns=%lu max=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)worst_ticks,
                   (unsigned long)worst_ns,
                   (unsigned long)c->max_latency_ns);
    VSF_TEST_ASSERT(worst_ns <= c->max_latency_ns);

    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_set_input(gpio, pin_mask);
}

#endif /* VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED */

/* EOF */
