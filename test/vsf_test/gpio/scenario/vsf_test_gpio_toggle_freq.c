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
#include "vsf_test_gpio_toggle_freq.h"

static vsf_test_gpio_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_gpio_toggle_freq_case_t __gpio_toggle_freq_cases[] = {
    VSF_TEST_GPIO_TOGGLE_FREQ_CASES_INIT
};

void vsf_test_gpio_toggle_freq_add_cases(vsf_gpio_t *gpio_instance)
{
    s_scenario.gpio_instance = gpio_instance;
    for (uint8_t i = 0; i < VSF_TEST_GPIO_TOGGLE_FREQ_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_toggle_freq_%u purpose=perf-freq pin=%u count=%lu",
            (unsigned)__gpio_toggle_freq_cases[i].idx,
            (unsigned)__gpio_toggle_freq_cases[i].pin,
            (unsigned long)__gpio_toggle_freq_cases[i].toggle_count);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_toggle_freq_run,
            __cfg_str_pool[i], (void *)&__gpio_toggle_freq_cases[i]);
    }
}

void vsf_test_gpio_toggle_freq_run(const vsf_test_gpio_toggle_freq_case_t *c)
{
    vsf_gpio_t *gpio = c->scenario->gpio_instance;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, pin_mask);

    vsf_systimer_tick_t start = vsf_systimer_get();
    for (uint32_t i = 0; i < c->toggle_count; i++) {
        vsf_gpio_toggle(gpio, pin_mask);
    }
    vsf_systimer_tick_t end = vsf_systimer_get();

    uint64_t total_us = vsf_systimer_tick_to_us(end - start);
    /* Avoid divide-by-zero on impossibly fast loops. */
    uint32_t period_ns = (c->toggle_count == 0) ? 0
                        : (uint32_t)((total_us * 1000ULL) / c->toggle_count);

    vsf_trace_info("GPIO:TOGGLE_FREQ:count=%lu total_us=%llu period_ns=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->toggle_count, (unsigned long long)total_us,
                   (unsigned long)period_ns);

    /* Sanity: period should be well under 1 µs on RP2040 @ 125 MHz.
     * Loosen to <= 5 µs to tolerate trace overhead and compiler differences. */
    VSF_TEST_ASSERT(period_ns < 5000);
}

#endif /* VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED */

/* EOF */
