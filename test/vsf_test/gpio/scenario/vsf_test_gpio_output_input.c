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
#include "vsf_test_gpio_output_input.h"

static vsf_test_gpio_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_test_gpio_output_input_case_t __gpio_output_input_cases[] = {
    VSF_TEST_GPIO_OUTPUT_INPUT_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_output_input_add_cases(vsf_gpio_t *gpio_instance)
{
    s_scenario.gpio_instance = gpio_instance;
    for (uint8_t i = 0; i < VSF_TEST_GPIO_OUTPUT_INPUT_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_output_input_%u purpose=loopback hw_req=gpio_loopback out=%u in=%u",
            (unsigned)__gpio_output_input_cases[i].idx,
            (unsigned)__gpio_output_input_cases[i].out_pin,
            (unsigned)__gpio_output_input_cases[i].in_pin);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_output_input_run,
            __cfg_str_pool[i], (void *)&__gpio_output_input_cases[i]);
    }
}

void vsf_test_gpio_output_input_run(const vsf_test_gpio_output_input_case_t *c)
{
    vsf_gpio_t *gpio = c->scenario->gpio_instance;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_capability_t cap = vsf_gpio_capability(gpio);
    VSF_TEST_ASSERT((cap.pin_mask & out_mask) != 0);
    VSF_TEST_ASSERT((cap.pin_mask & in_mask)  != 0);

    /* Configure pin A as push-pull output. */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Configure pin B as input with no pull (relies on the loopback wire). */
    err = vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

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
