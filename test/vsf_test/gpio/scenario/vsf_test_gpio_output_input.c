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

#include "vsf_test_gpio_output_input.h"

#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_gpio_output_input_case_t __gpio_output_input_cases[] = {
    VSF_TEST_GPIO_OUTPUT_INPUT_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_output_input_add_cases(vsf_test_gpio_output_input_suite_t *suite)
{
    suite->name    = "gpio_output_input";
    suite->purpose = "loopback";
    suite->hw_req  = "gpio_loopback";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_OUTPUT_INPUT_CASE_COUNT; i++) {
        __gpio_output_input_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_output_input_run,
            (void *)&__gpio_output_input_cases[i]);
    }
}

void vsf_test_gpio_output_input_run(const vsf_test_gpio_output_input_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    vsf_gpio_capability_t cap = vsf_gpio_capability(gpio);
    VSF_TEST_ASSERT((cap.pin_mask & out_mask) != 0);
    VSF_TEST_ASSERT((cap.pin_mask & in_mask)  != 0);

    /* Configure pin A as push-pull output. */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
     * get_pin_configuration() must report the output mode we just set.
     * Catches drivers that "accept" the cfg without actually applying it. */
    vsf_gpio_cfg_t got = {0};
    err = vsf_gpio_get_pin_configuration(gpio, c->out_pin, &got);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT((got.mode & VSF_GPIO_MODE_MASK) ==
                    (VSF_GPIO_OUTPUT_PUSH_PULL & VSF_GPIO_MODE_MASK));

    /* Configure pin B as input only if it's a different pin (otherwise we'd
     * overwrite the OUTPUT_PUSH_PULL config). Self-loopback exploits the
     * fact that RP2040 supports simultaneous SIO output + input on one pin. */
    if (c->in_pin != c->out_pin) {
        err = vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        });
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    }

    /* Drive high via write, observe via input pin. */
    vsf_gpio_write(gpio, out_mask, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == in_mask);
    VSF_TEST_ASSERT((vsf_gpio_read_output_register(gpio) & out_mask) == out_mask);

    /* Drive low via write. */
    vsf_gpio_write(gpio, out_mask, 0);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == 0);
    VSF_TEST_ASSERT((vsf_gpio_read_output_register(gpio) & out_mask) == 0);

    /* set() / clear() should behave the same as write(high/low). */
    vsf_gpio_set(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == in_mask);

    vsf_gpio_clear(gpio, out_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == 0);
}

#endif /* VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED */

/* EOF */
