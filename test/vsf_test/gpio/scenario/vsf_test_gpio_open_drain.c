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

#include "vsf_test_gpio_open_drain.h"

#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_open_drain_case_t __gpio_open_drain_cases[] = {
    VSF_TEST_GPIO_OPEN_DRAIN_CASES_INIT
};

void vsf_test_gpio_open_drain_add_cases(vsf_test_gpio_open_drain_scene_t *scene)
{
    scene->name    = "gpio_open_drain";
    scene->purpose = "od";
    scene->hw_req  = "gpio_loopback+pull_up";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_OPEN_DRAIN_CASE_COUNT; i++) {
        __gpio_open_drain_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_open_drain_run,
            (void *)&__gpio_open_drain_cases[i]);
    }
}

void vsf_test_gpio_open_drain_run(const vsf_test_gpio_open_drain_case_t *c)
{
    vsf_gpio_t *gpio = c->scene->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    /* Use internal pull-up on the input pin as a fallback when no
     * external resistor is wired (the PRD-mandated fixture). */
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_OPEN_DRAIN | VSF_GPIO_PULL_UP,
    });
    if (c->in_pin != c->out_pin) {
        vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_PULL_UP,
        });
    }

    /* OD writes 0 → actively drives low */
    vsf_gpio_write(gpio, out_mask, 0);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == 0);

    /* OD writes 1 → releases line, pull-up brings it high */
    vsf_gpio_write(gpio, out_mask, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == in_mask);
}

#endif /* VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED */

/* EOF */
