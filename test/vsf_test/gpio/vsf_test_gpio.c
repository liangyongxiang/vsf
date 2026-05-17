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

#define REG_IF(gate, s, field, add_fn)            \
    do {                                          \
        if (scenario_gateway(gate)) {             \
            add_fn(&(s)->field);                  \
        }                                         \
    } while (0)

void vsf_test_gpio_register_all(vsf_test_gpio_scenes_t *s)
{
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
    REG_IF("gpio_output_input",         s, output_input,      vsf_test_gpio_output_input_add_cases);
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
    REG_IF("gpio_toggle",               s, toggle,            vsf_test_gpio_toggle_add_cases);
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
    REG_IF("gpio_direction",            s, direction,         vsf_test_gpio_direction_add_cases);
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
    REG_IF("gpio_atomic",               s, atomic,            vsf_test_gpio_atomic_add_cases);
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
    REG_IF("gpio_pinmux",               s, pinmux,            vsf_test_gpio_pinmux_add_cases);
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
    REG_IF("gpio_multi_pin",            s, multi_pin,         vsf_test_gpio_multi_pin_add_cases);
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
    REG_IF("gpio_open_drain",           s, open_drain,        vsf_test_gpio_open_drain_add_cases);
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
    REG_IF("gpio_toggle_freq",          s, toggle_freq,       vsf_test_gpio_toggle_freq_add_cases);
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
    REG_IF("gpio_write_throughput",     s, write_throughput,  vsf_test_gpio_write_throughput_add_cases);
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
    REG_IF("gpio_toggle_stress",        s, toggle_stress,     vsf_test_gpio_toggle_stress_add_cases);
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    REG_IF("gpio_concurrent_prio",      s, concurrent_prio,   vsf_test_gpio_concurrent_prio_add_cases);
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    REG_IF("gpio_exti",                 s, exti,              vsf_test_gpio_exti_add_cases);
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    REG_IF("gpio_irq_latency",          s, irq_latency,       vsf_test_gpio_irq_latency_add_cases);
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    REG_IF("gpio_irq_lifecycle",        s, irq_lifecycle,     vsf_test_gpio_irq_lifecycle_add_cases);
#endif
}

/* EOF */
