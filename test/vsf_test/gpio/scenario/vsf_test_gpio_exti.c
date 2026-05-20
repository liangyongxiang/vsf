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

#define __VSF_TEST_GPIO_CLASS_IMPLEMENT
#include "vsf_test_gpio_exti.h"

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_exti_case_t __gpio_exti_cases[] = {
    VSF_TEST_GPIO_EXTI_CASES_INIT
};

static void __exti_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_test_gpio_exti_suite_t *suite = (vsf_test_gpio_exti_suite_t *)target;
    if (pin_mask & suite->expected_pin) {
        suite->count++;
    }
}

void vsf_test_gpio_exti_add_cases(vsf_test_gpio_exti_suite_t *suite)
{
    suite->name    = "gpio_exti";
    suite->purpose = "exti";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_EXTI_CASE_COUNT; i++) {
        __gpio_exti_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_exti_run,
            (void *)&__gpio_exti_cases[i]);
    }
}

void vsf_test_gpio_exti_run(const vsf_test_gpio_exti_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    /* Per-case state in suite: must be re-initialised before each run. */
    c->suite->count        = 0;
    c->suite->expected_pin = pin_mask;

    /* Configure as EXTI input on falling edge. */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_UP | VSF_GPIO_EXTI_MODE_FALLING,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __exti_handler,
        .target_ptr = c->suite,
        .prio       = vsf_arch_prio_highest,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_gpio_exti_irq_enable(gpio, pin_mask);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Self-trigger via SIO output on the same pin. The pin remains in SIO
     * mode (FUNCSEL=SIO) so we can drive it; EXTI sees the resulting edges. */
    vsf_gpio_set_output(gpio, pin_mask);
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    /* Clear stale event from initial setup. */
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    c->suite->count = 0;

    /* Drop low → falling edge → EXTI fires. */
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    uint32_t after_first = c->suite->count;
    VSF_TEST_ASSERT(after_first >= 1);

    /* Disable EXTI; toggle should NOT trigger more. */
    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_exti_irq_clear(gpio, pin_mask);  /* discard any latched */
    uint32_t after_disabled = c->suite->count;
    VSF_TEST_ASSERT(after_disabled == after_first);

    /* Re-enable, re-trigger. */
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    vsf_gpio_exti_irq_enable(gpio, pin_mask);
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT(c->suite->count > after_first);

    /* Verify get_configuration round-trips. */
    vsf_gpio_exti_irq_cfg_t got = {0};
    err = vsf_gpio_exti_irq_get_configuration(gpio, &got);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(got.handler_fn == __exti_handler);

    /* Cleanup. */
    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_set_input(gpio, pin_mask);

    vsf_trace_info("GPIO:EXTI:count=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->suite->count);
}

#endif /* VSF_TEST_GPIO_EXTI_ENABLE == ENABLED */

/* EOF */
