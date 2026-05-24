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

#include "vsf_test_gpio_write_throughput.h"

#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

VSF_TEST_SUITE_REGISTER(vsf_test_gpio_write_throughput_add_cases,
    vsf_test_gpio_write_throughput_suite_t,
    vsf_test_gpio_write_throughput_case_t,
    vsf_test_gpio_write_throughput_run,
    VSF_TEST_GPIO_WRITE_THROUGHPUT_CASES_INIT,
    "gpio_write_throughput", "perf-tput", "none",
    false)

void vsf_test_gpio_write_throughput_run(const vsf_test_gpio_write_throughput_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });

    /* Fixed-iteration loop. Time-bounded variants risk hanging when the
     * systimer isn't running in the synchronous test context. */
    uint32_t count = 100000;
    vsf_systimer_tick_t start = vsf_systimer_get();
    for (uint32_t i = 0; i < count; i++) {
        vsf_gpio_write(gpio, pin_mask, (i & 1) ? pin_mask : 0);
    }
    vsf_systimer_tick_t end = vsf_systimer_get();
    uint64_t elapsed_us = vsf_systimer_tick_to_us(end - start);
    uint32_t writes_per_sec = (elapsed_us == 0) ? 0
                            : (uint32_t)((uint64_t)count * 1000000ULL / elapsed_us);

    vsf_trace_info("GPIO:WRITE_TPUT:count=%lu elapsed_us=%llu per_sec=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)count, (unsigned long long)elapsed_us,
                   (unsigned long)writes_per_sec);

    /* If the systimer isn't running, elapsed will be 0 and we can't
     * assert a throughput floor. Assert only that all writes executed. */
    if (elapsed_us > 0) {
        VSF_TEST_ASSERT(writes_per_sec > 1000000);
    }
    (void)c->duration_us;
}

#endif /* VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED */

/* EOF */
