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

#include "vsf_test_gpio_atomic.h"

#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

/* Functional-only check. LA glitch detection lives host-side. */
void vsf_test_gpio_atomic_run(const vsf_test_gpio_atomic_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

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
