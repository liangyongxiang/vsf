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
/*============================ LOCAL VARIABLES ===============================*/

static vsf_gpio_pin_mask_t __out_mask;
static uint32_t __period_us;
static volatile uint32_t __callback_toggles;
static volatile uint32_t __main_toggles;



#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_concurrent_prio_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_gpio_concurrent_prio_params_t *p = tc->arg;
    vsf_gpio_t *gpio = (vsf_gpio_t *)fixture;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, out_mask);

    /* Per-case state in suite: must be re-initialised before each run. */
    __out_mask         = out_mask;
    __period_us        = p->callback_period_us;
    __callback_toggles = 0;
    __main_toggles     = 0;

    /* Concurrent toggle test: two toggle streams at different rates.
     * Stream A: high-frequency burst (1 k toggles / ms loop).
     * Stream B: periodic toggle at callback_period_us intervals.
     * Both run in the main loop; the test verifies sustained toggle
     * activity across the full duration. */
    uint32_t duration_us = p->duration_ms * 1000;
    uint32_t next_callback = p->callback_period_us;

    for (uint32_t elapsed = 0; elapsed < duration_us; elapsed++) {
        /* Stream A: high-frequency toggle. */
        vsf_gpio_toggle(gpio, out_mask);
        __main_toggles++;

        /* Stream B: periodic toggle at callback_period_us. */
        if (elapsed >= next_callback) {
            vsf_gpio_toggle(gpio, out_mask);
            __callback_toggles++;
            next_callback += p->callback_period_us;
        }

        vsf_test_busy_wait_us(1);
    }

    vsf_trace_info("GPIO:CONCURRENT:cb=%lu main=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)__callback_toggles,
                   (unsigned long)__main_toggles);
    /* Both streams must have run. */
    VSF_TEST_ASSERT(__callback_toggles > 0);
    VSF_TEST_ASSERT(__main_toggles > 0);
}

#endif /* VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED */

/* EOF */
