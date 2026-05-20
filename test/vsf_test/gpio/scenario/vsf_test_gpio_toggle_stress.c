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

#include "vsf_test_gpio_toggle_stress.h"

#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_toggle_stress_case_t __gpio_toggle_stress_cases[] = {
    VSF_TEST_GPIO_TOGGLE_STRESS_CASES_INIT
};

void vsf_test_gpio_toggle_stress_add_cases(vsf_test_gpio_toggle_stress_suite_t *suite)
{
    suite->name    = "gpio_toggle_stress";
    suite->purpose = "stress";
    suite->hw_req  = "gpio_loopback";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_TOGGLE_STRESS_CASE_COUNT; i++) {
        __gpio_toggle_stress_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_toggle_stress_run,
            (void *)&__gpio_toggle_stress_cases[i]);
    }
}

void vsf_test_gpio_toggle_stress_run(const vsf_test_gpio_toggle_stress_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    if (c->in_pin != c->out_pin) {
        vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        });
    }

    /* Seed: drive low. expected_high tracks the post-toggle level. */
    vsf_gpio_clear(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    bool expected_high = false;
    uint32_t miss = 0;
    for (uint32_t i = 0; i < c->stress_count; i++) {
        vsf_gpio_toggle(gpio, out_mask);
        expected_high = !expected_high;
        /* Read on every iteration so a missed edge is caught immediately. */
        vsf_gpio_pin_mask_t in_val = vsf_gpio_read(gpio) & in_mask;
        if (expected_high) {
            if (in_val != in_mask) { miss++; }
        } else {
            if (in_val != 0)        { miss++; }
        }
    }
    vsf_trace_info("GPIO:STRESS:count=%lu miss=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->stress_count, (unsigned long)miss);
    VSF_TEST_ASSERT(miss == 0);
}

#endif /* VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED */

/* EOF */
