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
#include "vsf_test_gpio_write_throughput.h"

static vsf_test_gpio_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_gpio_write_throughput_case_t __gpio_write_throughput_cases[] = {
    VSF_TEST_GPIO_WRITE_THROUGHPUT_CASES_INIT
};

void vsf_test_gpio_write_throughput_add_cases(vsf_gpio_t *gpio_instance)
{
    s_scenario.gpio_instance = gpio_instance;
    for (uint8_t i = 0; i < VSF_TEST_GPIO_WRITE_THROUGHPUT_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_write_throughput_%u purpose=perf-tput pin=%u dur_us=%lu",
            (unsigned)__gpio_write_throughput_cases[i].idx,
            (unsigned)__gpio_write_throughput_cases[i].pin,
            (unsigned long)__gpio_write_throughput_cases[i].duration_us);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_write_throughput_run,
            __cfg_str_pool[i], (void *)&__gpio_write_throughput_cases[i]);
    }
}

void vsf_test_gpio_write_throughput_run(const vsf_test_gpio_write_throughput_case_t *c)
{
    vsf_gpio_t *gpio = c->scenario->gpio_instance;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });

    vsf_systimer_tick_t start = vsf_systimer_get();
    uint32_t count = 0;
    uint64_t deadline_us = (uint64_t)c->duration_us;
    while (1) {
        vsf_gpio_write(gpio, pin_mask, (count & 1) ? pin_mask : 0);
        count++;
        if ((count & 0x3FFu) == 0) {
            uint64_t elapsed = vsf_systimer_tick_to_us(vsf_systimer_get() - start);
            if (elapsed >= deadline_us) {
                break;
            }
        }
    }
    uint64_t elapsed_us = vsf_systimer_tick_to_us(vsf_systimer_get() - start);
    uint32_t writes_per_sec = (elapsed_us == 0) ? 0
                            : (uint32_t)((uint64_t)count * 1000000ULL / elapsed_us);

    vsf_trace_info("GPIO:WRITE_TPUT:count=%lu elapsed_us=%llu per_sec=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)count, (unsigned long long)elapsed_us,
                   (unsigned long)writes_per_sec);

    /* Loose lower bound: > 1 MHz writes/sec on RP2040 @ 125 MHz. */
    VSF_TEST_ASSERT(writes_per_sec > 1000000);
}

#endif /* VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED */

/* EOF */
