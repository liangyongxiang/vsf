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

#include "vsf_test_gpio_direction.h"

#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_direction_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_gpio_direction_params_t *p = tc->arg;
    vsf_gpio_t *gpio = (vsf_gpio_t *)fixture;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    /* Configure as output and verify direction reads back as output */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT((vsf_gpio_get_direction(gpio, pin_mask) & pin_mask) == pin_mask);

    /* Switch direction → expect input */
    vsf_gpio_switch_direction(gpio, pin_mask);
    VSF_TEST_ASSERT((vsf_gpio_get_direction(gpio, pin_mask) & pin_mask) == 0);

    /* Switch again → back to output */
    vsf_gpio_switch_direction(gpio, pin_mask);
    VSF_TEST_ASSERT((vsf_gpio_get_direction(gpio, pin_mask) & pin_mask) == pin_mask);

    /* set_input / set_output explicit calls */
    vsf_gpio_set_input(gpio, pin_mask);
    VSF_TEST_ASSERT((vsf_gpio_get_direction(gpio, pin_mask) & pin_mask) == 0);
    vsf_gpio_set_output(gpio, pin_mask);
    VSF_TEST_ASSERT((vsf_gpio_get_direction(gpio, pin_mask) & pin_mask) == pin_mask);

    /* get_pin_configuration round-trip: configure as input pull-up, verify */
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_PULL_UP,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_gpio_cfg_t got = { 0 };
    err = vsf_gpio_get_pin_configuration(gpio, p->pin, &got);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT((got.mode & VSF_GPIO_MODE_MASK)         == VSF_GPIO_INPUT);
    VSF_TEST_ASSERT((got.mode & VSF_GPIO_PULL_UP_DOWN_MASK) == VSF_GPIO_PULL_UP);
}

#endif /* VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED */

/* EOF */
