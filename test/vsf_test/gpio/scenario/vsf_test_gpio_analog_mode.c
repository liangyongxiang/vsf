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

#include "vsf_test_gpio_analog_mode.h"

#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_analog_mode_case_t __gpio_analog_mode_cases[] = {
    VSF_TEST_GPIO_ANALOG_MODE_CASES_INIT
};

void vsf_test_gpio_analog_mode_add_cases(vsf_test_gpio_analog_mode_suite_t *suite)
{
    suite->name    = "gpio_analog_mode";
    suite->purpose = "analog";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_ANALOG_MODE_CASE_COUNT; i++) {
        __gpio_analog_mode_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_analog_mode_run,
            (void *)&__gpio_analog_mode_cases[i]);
    }
}

/* VSF_GPIO_ANALOG must disconnect the digital input buffer (PADS.IE=0,
 * FUNCSEL=NULL on RP2040). With the internal pull-up enabled, the pad
 * voltage is HIGH, yet vsf_gpio_read() must report 0 — proving the read
 * path is truly cut from the pad. With pull-down enabled, read() also
 * returns 0 (sanity check, line is low anyway).
 *
 * After re-configuring as INPUT with pull-up, read() must report 1 —
 * confirming we can recover the digital input path on the same pin. */
void vsf_test_gpio_analog_mode_run(const vsf_test_gpio_analog_mode_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    /* ANALOG mode with internal pull-up. The pad is electrically high,
     * but the input buffer is OFF so vsf_gpio_read() must report 0. */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_ANALOG | VSF_GPIO_PULL_UP,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & pin_mask) == 0);

    /* ANALOG mode with pull-down — also reads 0. */
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_ANALOG | VSF_GPIO_PULL_DOWN,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & pin_mask) == 0);

    /* Lifecycle: switch back to digital INPUT with pull-up. The input
     * buffer must come back on — read() now reports 1. */
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_PULL_UP,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & pin_mask) == pin_mask);

    /* get_pin_configuration round-trip: after switching back to ANALOG,
     * the read-back mode should reflect ANALOG. */
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_ANALOG | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_gpio_cfg_t got = { 0 };
    err = vsf_gpio_get_pin_configuration(gpio, c->pin, &got);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT((got.mode & VSF_GPIO_MODE_MASK) == VSF_GPIO_ANALOG);

    /* Cleanup: leave the pin as plain INPUT so the next suite starts clean. */
    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });
}

#endif /* VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED */

/* EOF */
