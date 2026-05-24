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

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED


static vsf_test_gpio_irq_latency_case_t __gpio_irq_latency_cases[] = {
    VSF_TEST_GPIO_IRQ_LATENCY_CASES_INIT
};

static void __latency_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_test_gpio_irq_latency_suite_t *suite = (vsf_test_gpio_irq_latency_suite_t *)target;
    if (pin_mask & suite->expected_pin) {
        suite->isr_tick = vsf_systimer_get();
        suite->fired = true;
    }
}

void vsf_test_gpio_irq_latency_add_cases(vsf_test_gpio_irq_latency_suite_t *suite)
{
    suite->name    = "gpio_irq_latency";
    suite->purpose = "perf-irq";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_IRQ_LATENCY_CASE_COUNT; i++) {
        __gpio_irq_latency_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_irq_latency_run,
            (void *)&__gpio_irq_latency_cases[i]);
    }
}

void vsf_test_gpio_irq_latency_run(const vsf_test_gpio_irq_latency_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    /* Per-case state in suite: must be re-initialised before each run. */
    c->suite->expected_pin = pin_mask;
    c->suite->fired        = false;
    c->suite->isr_tick     = 0;
    c->suite->trigger_tick = 0;

    /* Configure pin as EXTI rising edge — driven by SIO from the same test
     * (self-trigger; no external wiring needed). */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_DOWN | VSF_GPIO_EXTI_MODE_RISING,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __latency_handler,
        .target_ptr = c->suite,
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
        c->suite->fired = false;
        c->suite->isr_tick = 0;
        vsf_gpio_clear(gpio, pin_mask);
        vsf_test_busy_wait_ms(1);
        vsf_gpio_exti_irq_clear(gpio, pin_mask);

        c->suite->trigger_tick = vsf_systimer_get();
        vsf_gpio_set(gpio, pin_mask);   /* rising edge → EXTI fires */
        /* Spin until ISR captures its tick. */
        for (uint32_t spin = 0; spin < 100000 && !c->suite->fired; spin++) {
            __asm__ volatile("nop");
        }
        VSF_TEST_ASSERT(c->suite->fired);
        uint32_t delta = (uint32_t)(c->suite->isr_tick - c->suite->trigger_tick);
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
