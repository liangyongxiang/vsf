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
#include "vsf_test_gpio_atomic.h"

static vsf_test_gpio_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_gpio_atomic_case_t __gpio_atomic_cases[] = {
    VSF_TEST_GPIO_ATOMIC_CASES_INIT
};

void vsf_test_gpio_atomic_add_cases(vsf_gpio_t *gpio_instance)
{
    s_scenario.gpio_instance = gpio_instance;
    for (uint8_t i = 0; i < VSF_TEST_GPIO_ATOMIC_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_atomic_%u purpose=atomic hw_req=gpio_loopback+la out=%u in=%u",
            (unsigned)__gpio_atomic_cases[i].idx,
            (unsigned)__gpio_atomic_cases[i].out_pin,
            (unsigned)__gpio_atomic_cases[i].in_pin);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_atomic_run,
            __cfg_str_pool[i], (void *)&__gpio_atomic_cases[i]);
    }
}

/* Functional-only check. LA glitch detection lives host-side. */
void vsf_test_gpio_atomic_run(const vsf_test_gpio_atomic_case_t *c)
{
    vsf_gpio_t *gpio = c->scenario->gpio_instance;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_capability_t cap = vsf_gpio_capability(gpio);
    VSF_TEST_ASSERT(cap.support_output_and_set);
    VSF_TEST_ASSERT(cap.support_output_and_clear);

    vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });

    /* output_and_set: transition input → output-high atomically */
    vsf_gpio_output_and_set(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == in_mask);
    VSF_TEST_ASSERT((vsf_gpio_get_direction(gpio, out_mask) & out_mask) == out_mask);

    /* Switch back to input */
    vsf_gpio_set_input(gpio, out_mask);

    /* output_and_clear: transition input → output-low atomically */
    vsf_gpio_output_and_clear(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == 0);
    VSF_TEST_ASSERT((vsf_gpio_get_direction(gpio, out_mask) & out_mask) == out_mask);
}

#endif /* VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED */

/* EOF */
