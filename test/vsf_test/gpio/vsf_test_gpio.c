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

#include "component/test/vsf_test/vsf_test.h"
#include "vsf_test_gpio.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
void vsf_test_gpio_init(vsf_test_gpio_suites_t *s, const vsf_test_gpio_cfg_t *cfg)
{
    s->output_input.gpio     = cfg->gpio;
    s->toggle.gpio           = cfg->gpio;
    s->direction.gpio        = cfg->gpio;
    s->atomic.gpio           = cfg->gpio;
    s->pinmux.gpio           = cfg->gpio;
    s->multi_pin.gpio        = cfg->gpio;
    s->open_drain.gpio       = cfg->gpio;
    s->toggle_freq.gpio      = cfg->gpio;
    s->write_throughput.gpio = cfg->gpio;
    s->toggle_stress.gpio    = cfg->gpio;
    s->concurrent_prio.gpio  = cfg->gpio;
    s->exti.gpio             = cfg->gpio;
    s->irq_latency.gpio      = cfg->gpio;
    s->irq_lifecycle.gpio    = cfg->gpio;
    s->systimer_health.gpio  = cfg->gpio;
    s->analog_mode.gpio      = cfg->gpio;
    s->io_check.gpio         = cfg->gpio;
    s->pinmux.usart          = cfg->usart;
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
    vsf_test_gpio_output_input_add_cases(&s->output_input);
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
    vsf_test_gpio_toggle_add_cases(&s->toggle);
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
    vsf_test_gpio_direction_add_cases(&s->direction);
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
    vsf_test_gpio_atomic_add_cases(&s->atomic);
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
    vsf_test_gpio_pinmux_add_cases(&s->pinmux);
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
    vsf_test_gpio_multi_pin_add_cases(&s->multi_pin);
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
    vsf_test_gpio_open_drain_add_cases(&s->open_drain);
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
    vsf_test_gpio_toggle_freq_add_cases(&s->toggle_freq);
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
    vsf_test_gpio_write_throughput_add_cases(&s->write_throughput);
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
    vsf_test_gpio_toggle_stress_add_cases(&s->toggle_stress);
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    vsf_test_gpio_concurrent_prio_add_cases(&s->concurrent_prio);
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    vsf_test_gpio_exti_add_cases(&s->exti);
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    vsf_test_gpio_irq_latency_add_cases(&s->irq_latency);
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    vsf_test_gpio_irq_lifecycle_add_cases(&s->irq_lifecycle);
#endif
#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
    vsf_test_gpio_systimer_health_add_cases(&s->systimer_health);
#endif
#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
    vsf_test_gpio_analog_mode_add_cases(&s->analog_mode);
#endif
#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
    vsf_test_gpio_io_check_add_cases(&s->io_check);
#endif
}

/* EOF */
