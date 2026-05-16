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
#include "vsf_test_gpio_multi_pin.h"

static vsf_test_gpio_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_gpio_multi_pin_case_t __gpio_multi_pin_cases[] = {
    VSF_TEST_GPIO_MULTI_PIN_CASES_INIT
};

void vsf_test_gpio_multi_pin_add_cases(vsf_gpio_t *gpio_instance)
{
    s_scenario.gpio_instance = gpio_instance;
    for (uint8_t i = 0; i < VSF_TEST_GPIO_MULTI_PIN_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][96];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_multi_pin_%u purpose=multi-pair hw_req=gpio_loopback(>=4) outA=%u inA=%u outB=%u inB=%u",
            (unsigned)__gpio_multi_pin_cases[i].idx,
            (unsigned)__gpio_multi_pin_cases[i].out_pin_a,
            (unsigned)__gpio_multi_pin_cases[i].in_pin_a,
            (unsigned)__gpio_multi_pin_cases[i].out_pin_b,
            (unsigned)__gpio_multi_pin_cases[i].in_pin_b);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_multi_pin_run,
            __cfg_str_pool[i], (void *)&__gpio_multi_pin_cases[i]);
    }
}

void vsf_test_gpio_multi_pin_run(const vsf_test_gpio_multi_pin_case_t *c)
{
    vsf_gpio_t *gpio = c->scenario->gpio_instance;
    vsf_gpio_pin_mask_t oa = (vsf_gpio_pin_mask_t)1u << c->out_pin_a;
    vsf_gpio_pin_mask_t ob = (vsf_gpio_pin_mask_t)1u << c->out_pin_b;
    vsf_gpio_pin_mask_t ia = (vsf_gpio_pin_mask_t)1u << c->in_pin_a;
    vsf_gpio_pin_mask_t ib = (vsf_gpio_pin_mask_t)1u << c->in_pin_b;
    vsf_gpio_pin_mask_t out_mask = oa | ob;
    vsf_gpio_pin_mask_t in_mask  = ia | ib;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });

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
