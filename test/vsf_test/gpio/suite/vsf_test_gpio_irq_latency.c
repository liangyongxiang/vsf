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

#define __VSF_TEST_GPIO_CLASS_IMPLEMENT
#include "vsf_test_gpio_irq_latency.h"

/*============================ LOCAL VARIABLES ===============================*/

static vsf_gpio_pin_mask_t __expected_pin;
static volatile vsf_systimer_tick_t __isr_tick;
static volatile bool __fired;
static vsf_systimer_tick_t __trigger_tick;

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED

static void __latency_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_test_suite_t *suite = target;
    if (pin_mask & __expected_pin) {
        __isr_tick = vsf_systimer_get();
        __fired = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_irq_latency_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_gpio_irq_latency_params_t *p = tc->arg;
    vsf_gpio_t *gpio = (vsf_gpio_t *)fixture;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    /* Per-case state in suite: must be re-initialised before each run. */
    __expected_pin = pin_mask;
    __fired        = false;
    __isr_tick     = 0;
    __trigger_tick = 0;

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
        __fired = false;
        __isr_tick = 0;
        vsf_gpio_clear(gpio, pin_mask);
        vsf_test_busy_wait_ms(1);
        vsf_gpio_exti_irq_clear(gpio, pin_mask);

        __trigger_tick = vsf_systimer_get();
        vsf_gpio_set(gpio, pin_mask);   /* rising edge → EXTI fires */
        /* Spin until ISR captures its tick. */
        for (uint32_t spin = 0; spin < 100000 && !__fired; spin++) {
            __asm__ volatile("nop");
        }
        VSF_TEST_ASSERT(__fired);
        uint32_t delta = (uint32_t)(__isr_tick - __trigger_tick);
        if (delta > worst_ticks) { worst_ticks = delta; }
    }

    uint64_t worst_us = vsf_systimer_tick_to_us(worst_ticks);
    uint32_t worst_ns = (uint32_t)(worst_us * 1000ULL);

    vsf_trace_info("GPIO:IRQ_LATENCY:worst_ticks=%lu worst_ns=%lu max=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)worst_ticks,
                   (unsigned long)worst_ns,
                   (unsigned long)p->max_latency_ns);
    VSF_TEST_ASSERT(worst_ns <= p->max_latency_ns);

    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_set_input(gpio, pin_mask);
}

#endif /* VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED */

/* EOF */
