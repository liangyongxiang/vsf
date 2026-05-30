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
#include "vsf_test_gpio_concurrent_prio.h"

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_concurrent_prio_run(const vsf_test_gpio_concurrent_prio_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, out_mask);

    /* Per-case state in suite: must be re-initialised before each run. */
    c->suite->out_mask         = out_mask;
    c->suite->period_us        = c->callback_period_us;
    c->suite->callback_toggles = 0;
    c->suite->main_toggles     = 0;

    /* Concurrent toggle test: two toggle streams at different rates.
     * Stream A: high-frequency burst (1 k toggles / ms loop).
     * Stream B: periodic toggle at callback_period_us intervals.
     * Both run in the main loop; the test verifies sustained toggle
     * activity across the full duration. */
    uint32_t duration_us = c->duration_ms * 1000;
    uint32_t next_callback = c->callback_period_us;

    for (uint32_t elapsed = 0; elapsed < duration_us; elapsed++) {
        /* Stream A: high-frequency toggle. */
        vsf_gpio_toggle(gpio, out_mask);
        c->suite->main_toggles++;

        /* Stream B: periodic toggle at callback_period_us. */
        if (elapsed >= next_callback) {
            vsf_gpio_toggle(gpio, out_mask);
            c->suite->callback_toggles++;
            next_callback += c->callback_period_us;
        }

        vsf_test_busy_wait_us(1);
    }

    vsf_trace_info("GPIO:CONCURRENT:cb=%lu main=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->suite->callback_toggles,
                   (unsigned long)c->suite->main_toggles);
    /* Both streams must have run. */
    VSF_TEST_ASSERT(c->suite->callback_toggles > 0);
    VSF_TEST_ASSERT(c->suite->main_toggles > 0);
}

#endif /* VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED */

/* EOF */
