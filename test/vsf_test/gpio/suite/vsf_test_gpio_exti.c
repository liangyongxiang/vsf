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
#include "vsf_test_suites.h"

/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED

static void __exti_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)

{
    vsf_test_suite_t *suite = target;
    if (pin_mask & vsf_test_suite_data.gpio_exti.expected_pin) {
        vsf_test_suite_data.gpio_exti.count++;
        if (vsf_test_suite_data.gpio_exti.disable_on_fire) {
            /* Level-trigger ISR storm guard. Disable the source after the
             * first hit; the main thread re-enables if it wants more. */
            vsf_gpio_exti_irq_disable(gpio, vsf_test_suite_data.gpio_exti.expected_pin);
        }
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_exti_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_gpio_exti_params_t *p = tc->arg;
    vsf_gpio_t *gpio = (vsf_gpio_t *)fixture;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << p->in_pin;
    bool self_loopback = (p->out_pin == p->in_pin);

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    bool active_low = (p->trigger_mode == VSF_GPIO_EXTI_MODE_FALLING
                    || p->trigger_mode == VSF_GPIO_EXTI_MODE_LOW_LEVEL);
    bool level_trig = (p->trigger_mode == VSF_GPIO_EXTI_MODE_LOW_LEVEL
                    || p->trigger_mode == VSF_GPIO_EXTI_MODE_HIGH_LEVEL);

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.gpio_exti.count           = 0;
    vsf_test_suite_data.gpio_exti.expected_pin    = in_mask;
    vsf_test_suite_data.gpio_exti.disable_on_fire = level_trig;

    /* Pre-park the output pin at the IDLE level (opposite of "active") so
     * configuring EXTI does not see a spurious trigger. */
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    if (active_low) {
        vsf_gpio_set(gpio, out_mask);
    } else {
        vsf_gpio_clear(gpio, out_mask);
    }
    vsf_test_busy_wait_ms(1);

    /* Configure in_pin as EXTI input with the requested trigger mode.
     * For self-loopback, RP2040 lets SIO continue driving the pin while
     * EXTI watches transitions. */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_UP | p->trigger_mode,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __exti_handler,
        .target_ptr = NULL,
        .prio       = vsf_arch_prio_highest,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* For self-loopback, restore SIO output on the same pin. */
    if (self_loopback) {
        vsf_gpio_set_output(gpio, in_mask);
    }

    err = vsf_gpio_exti_irq_enable(gpio, in_mask);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Clear any pending edge that the mode switch produced. */
    vsf_gpio_exti_irq_clear(gpio, in_mask);
    vsf_test_suite_data.gpio_exti.count = 0;

    /* Drive to ACTIVE state — trigger one event (edge) or sustained ISRs
     * (level, but handler self-disables after first hit). */
    if (active_low) {
        vsf_gpio_clear(gpio, out_mask);
    } else {
        vsf_gpio_set(gpio, out_mask);
    }
    vsf_test_busy_wait_ms(1);
    uint32_t after_active = vsf_test_suite_data.gpio_exti.count;
    VSF_TEST_ASSERT(after_active >= 1);

    /* For dual-edge mode, also drive the opposite transition and expect
     * a second hit. */
    if (p->trigger_mode == VSF_GPIO_EXTI_MODE_RISING_FALLING) {
        vsf_gpio_exti_irq_clear(gpio, in_mask);
        if (active_low) {
            vsf_gpio_set(gpio, out_mask);
        } else {
            vsf_gpio_clear(gpio, out_mask);
        }
        vsf_test_busy_wait_ms(1);
        VSF_TEST_ASSERT(vsf_test_suite_data.gpio_exti.count > after_active);
    }

    /* For edge-triggered (single-edge) modes, also verify the IRQ truly
     * stays disabled after vsf_gpio_exti_irq_disable: drive the opposite
     * edge then the active edge again, expect no count change. */
    if (!level_trig && p->trigger_mode != VSF_GPIO_EXTI_MODE_RISING_FALLING) {
        vsf_gpio_exti_irq_disable(gpio, in_mask);
        uint32_t baseline = vsf_test_suite_data.gpio_exti.count;
        /* idle → active → idle → active (extra noise) */
        if (active_low) {
            vsf_gpio_set(gpio, out_mask);
            vsf_test_busy_wait_ms(1);
            vsf_gpio_clear(gpio, out_mask);
        } else {
            vsf_gpio_clear(gpio, out_mask);
            vsf_test_busy_wait_ms(1);
            vsf_gpio_set(gpio, out_mask);
        }
        vsf_test_busy_wait_ms(1);
        vsf_gpio_exti_irq_clear(gpio, in_mask);
        VSF_TEST_ASSERT(vsf_test_suite_data.gpio_exti.count == baseline);
    }

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
     * get_configuration() round-trip. */
    vsf_gpio_exti_irq_cfg_t got = {0};
    err = vsf_gpio_exti_irq_get_configuration(gpio, &got);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(got.handler_fn == __exti_handler);

    /* Cleanup. */
    vsf_gpio_exti_irq_disable(gpio, in_mask);
    vsf_gpio_set_input(gpio, in_mask);

    vsf_trace_info("GPIO:EXTI:trigger=0x%x count=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned)p->trigger_mode, (unsigned long)vsf_test_suite_data.gpio_exti.count);
}

#endif /* VSF_TEST_GPIO_EXTI_ENABLE == ENABLED */

/* EOF */