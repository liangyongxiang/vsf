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

#include "vsf_test_gpio_systimer_health.h"

#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_systimer_health_case_t __gpio_systimer_health_cases[] = {
    VSF_TEST_GPIO_SYSTIMER_HEALTH_CASES_INIT
};

void vsf_test_gpio_systimer_health_add_cases(vsf_test_gpio_systimer_health_suite_t *suite)
{
    suite->name    = "gpio_systimer_health";
    suite->purpose = "health";
    suite->hw_req  = "la";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT; i++) {
        __gpio_systimer_health_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_systimer_health_run,
            (void *)&__gpio_systimer_health_cases[i]);
    }
}

/* Toggle a GPIO at known systimer-based intervals. The LA's sample clock
 * gives an external reference: the host decode counts edges and asserts
 * the on-wire period matches `interval_ms` within tolerance.
 *
 * If the RP2040 watchdog tick is misconfigured (see [[rp2040-watchdog-tick-required]]),
 * the timer block runs at ~10 kHz instead of 1 MHz and the on-wire interval
 * stretches by ~100×. The LA decode catches that. The firmware-side trace
 * cannot, because it compares the systimer against itself.
 */
void vsf_test_gpio_systimer_health_run(const vsf_test_gpio_systimer_health_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);

    vsf_systimer_tick_t start = vsf_systimer_get();
    for (uint32_t i = 0; i < c->toggle_count; i++) {
        vsf_test_busy_wait_ms(c->interval_ms);
        vsf_gpio_toggle(gpio, pin_mask);
    }
    vsf_systimer_tick_t end = vsf_systimer_get();

    /* Diagnostic only — firmware cannot self-detect a broken systimer.
     * The host decode is the authoritative pass/fail. */
    uint64_t total_us = vsf_systimer_tick_to_us(end - start);
    vsf_trace_info("GPIO:SYSTIMER_HEALTH:interval_ms=%lu toggle_count=%lu total_us=%llu"
                   VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->interval_ms,
                   (unsigned long)c->toggle_count,
                   (unsigned long long)total_us);
}

#endif /* VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED */

/* EOF */
