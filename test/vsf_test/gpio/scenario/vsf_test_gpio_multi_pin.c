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

#include "vsf_test_gpio_multi_pin.h"

#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_multi_pin_case_t __gpio_multi_pin_cases[] = {
    VSF_TEST_GPIO_MULTI_PIN_CASES_INIT
};

void vsf_test_gpio_multi_pin_add_cases(vsf_test_gpio_multi_pin_suite_t *suite)
{
    suite->name    = "gpio_multi_pin";
    suite->purpose = "multi-pair";
    suite->hw_req  = "gpio_loopback(>=4)";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_MULTI_PIN_CASE_COUNT; i++) {
        __gpio_multi_pin_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_multi_pin_run,
            (void *)&__gpio_multi_pin_cases[i]);
    }
}

void vsf_test_gpio_multi_pin_run(const vsf_test_gpio_multi_pin_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t oa = (vsf_gpio_pin_mask_t)1u << c->out_pin_a;
    vsf_gpio_pin_mask_t ob = (vsf_gpio_pin_mask_t)1u << c->out_pin_b;
    vsf_gpio_pin_mask_t ia = (vsf_gpio_pin_mask_t)1u << c->in_pin_a;
    vsf_gpio_pin_mask_t ib = (vsf_gpio_pin_mask_t)1u << c->in_pin_b;
    vsf_gpio_pin_mask_t out_mask = oa | ob;
    vsf_gpio_pin_mask_t in_mask  = ia | ib;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    /* For self-loopback (same pins for in/out), skip the input config so
     * the OUTPUT mode survives. */
    if ((c->in_pin_a != c->out_pin_a) || (c->in_pin_b != c->out_pin_b)) {
        vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        });
    }

    /* All 4 patterns 00 / 01 / 10 / 11 */
    struct { vsf_gpio_pin_mask_t out_val; vsf_gpio_pin_mask_t expect_in; } steps[] = {
        {0,      0},
        {oa,     ia},
        {ob,     ib},
        {oa|ob,  ia|ib},
    };
    for (size_t k = 0; k < sizeof(steps)/sizeof(steps[0]); k++) {
        vsf_gpio_write(gpio, out_mask, steps[k].out_val);
        vsf_test_busy_wait_ms(1);
        VSF_TEST_ASSERT((vsf_gpio_read(gpio) & in_mask) == steps[k].expect_in);
    }
}

#endif /* VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED */

/* EOF */
