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

#include "vsf_test_gpio_toggle.h"

#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_toggle_case_t __gpio_toggle_cases[] = {
    VSF_TEST_GPIO_TOGGLE_CASES_INIT
};

void vsf_test_gpio_toggle_add_cases(vsf_test_gpio_toggle_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_GPIO_TOGGLE_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_toggle_%u purpose=toggle hw_req=gpio_loopback out=%u in=%u",
            (unsigned)__gpio_toggle_cases[i].idx,
            (unsigned)__gpio_toggle_cases[i].out_pin,
            (unsigned)__gpio_toggle_cases[i].in_pin);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_toggle_run,
            __cfg_str_pool[i], (void *)&__gpio_toggle_cases[i]);
        __gpio_toggle_cases[i].scene = scene;
    }
}

void vsf_test_gpio_toggle_run(const vsf_test_gpio_toggle_case_t *c)
{
    vsf_gpio_t *gpio = c->scene->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    if (c->in_pin != c->out_pin) {
        vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        });
    }

    /* Seed known state: drive low, then verify the toggle pattern */
    vsf_gpio_clear(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == 0);

    vsf_gpio_set(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == in_mask);

    vsf_gpio_toggle(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == 0);

    vsf_gpio_toggle(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == in_mask);
}

#endif /* VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED */

/* EOF */
