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
#include "vsf_test_gpio_irq_lifecycle.h"

#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_irq_lifecycle_case_t __gpio_irq_lifecycle_cases[] = {
    VSF_TEST_GPIO_IRQ_LIFECYCLE_CASES_INIT
};

static void __lifecycle_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_test_gpio_irq_lifecycle_scene_t *scene = (vsf_test_gpio_irq_lifecycle_scene_t *)target;
    if (pin_mask & scene->lifecycle_pin) {
        scene->lifecycle_count++;
    }
}

void vsf_test_gpio_irq_lifecycle_add_cases(vsf_test_gpio_irq_lifecycle_scene_t *scene)
{
    scene->name    = "gpio_irq_lifecycle";
    scene->purpose = "irq-lifecycle";
    scene->hw_req  = "none";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_IRQ_LIFECYCLE_CASE_COUNT; i++) {
        __gpio_irq_lifecycle_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_irq_lifecycle_run,
            (void *)&__gpio_irq_lifecycle_cases[i]);
    }
}

void vsf_test_gpio_irq_lifecycle_run(const vsf_test_gpio_irq_lifecycle_case_t *c)
{
    vsf_gpio_t *gpio = c->scene->gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    /* Per-case state in scene: must be re-initialised before each run. */
    c->scene->lifecycle_pin   = pin_mask;
    c->scene->lifecycle_count = 0;

    /* config rising-edge */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_DOWN | VSF_GPIO_EXTI_MODE_RISING,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __lifecycle_handler,
        .target_ptr = c->scene,
        .prio       = vsf_arch_prio_highest,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* get_configuration round-trip */
    vsf_gpio_exti_irq_cfg_t got = {0};
    err = vsf_gpio_exti_irq_get_configuration(gpio, &got);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(got.handler_fn == __lifecycle_handler);

    /* Set the pin low, enable, then rising-edge trigger → count = 1. */
    vsf_gpio_set_output(gpio, pin_mask);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    c->scene->lifecycle_count = 0;
    err = vsf_gpio_exti_irq_enable(gpio, pin_mask);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_gpio_set(gpio, pin_mask);          /* rising edge */
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT(c->scene->lifecycle_count == 1);

    /* Disable → next edge must NOT increment. */
    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT(c->scene->lifecycle_count == 1);

    /* Re-enable → next edge increments to 2. */
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    err = vsf_gpio_exti_irq_enable(gpio, pin_mask);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_ms(1);
    VSF_TEST_ASSERT(c->scene->lifecycle_count == 2);

    /* Clear API: write 1 to any pending bits, returns pre-clear mask. */
    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_set_input(gpio, pin_mask);
    /* No pending after a clean disable+input switch — returning 0 here is OK. */
    (void)vsf_gpio_exti_irq_clear(gpio, pin_mask);

    vsf_trace_info("GPIO:IRQ_LIFECYCLE:count=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->scene->lifecycle_count);
}

#endif /* VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED */

/* EOF */
