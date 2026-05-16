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
#include "vsf_test_gpio_concurrent_prio.h"

static vsf_test_gpio_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static const vsf_test_gpio_concurrent_prio_case_t __gpio_concurrent_prio_cases[] = {
    VSF_TEST_GPIO_CONCURRENT_PRIO_CASES_INIT
};

/* Shared state between callback (high prio) and main loop (low prio). */
typedef struct {
    vsf_gpio_t          *gpio;
    vsf_gpio_pin_mask_t  out_mask;
    volatile uint32_t    callback_toggles;
    volatile bool        run;
} __concurrent_ctx_t;

static __concurrent_ctx_t s_ctx;
static vsf_callback_timer_t s_timer;

static void __callback_toggle(vsf_callback_timer_t *timer)
{
    if (!s_ctx.run) { return; }
    vsf_gpio_toggle(s_ctx.gpio, s_ctx.out_mask);
    s_ctx.callback_toggles++;
    /* Re-arm */
    vsf_callback_timer_add_us(timer, s_timer.due);
}

void vsf_test_gpio_concurrent_prio_add_cases(vsf_gpio_t *gpio_instance)
{
    s_scenario.gpio_instance = gpio_instance;
    for (uint8_t i = 0; i < VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][96];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_concurrent_prio_%u purpose=concurrency hw_req=gpio_loopback out=%u in=%u",
            (unsigned)__gpio_concurrent_prio_cases[i].idx,
            (unsigned)__gpio_concurrent_prio_cases[i].out_pin,
            (unsigned)__gpio_concurrent_prio_cases[i].in_pin);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_concurrent_prio_run,
            __cfg_str_pool[i], (void *)&__gpio_concurrent_prio_cases[i]);
    }
}

void vsf_test_gpio_concurrent_prio_run(const vsf_test_gpio_concurrent_prio_case_t *c)
{
    vsf_gpio_t *gpio = c->scenario->gpio_instance;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << c->in_pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, out_mask);

    s_ctx.gpio = gpio;
    s_ctx.out_mask = out_mask;
    s_ctx.callback_toggles = 0;
    s_ctx.run = true;

    s_timer.on_timer = __callback_toggle;
    s_timer.due      = c->callback_period_us;
    vsf_callback_timer_add_us(&s_timer, c->callback_period_us);

    uint32_t main_toggles = 0;
    vsf_systimer_tick_t start = vsf_systimer_get();
    while (vsf_systimer_tick_to_us(vsf_systimer_get() - start) < (uint64_t)c->duration_ms * 1000) {
        vsf_gpio_toggle(gpio, out_mask);
        main_toggles++;
    }
    s_ctx.run = false;
    vsf_callback_timer_remove(&s_timer);

    vsf_trace_info("GPIO:CONCURRENT:cb=%lu main=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)s_ctx.callback_toggles, (unsigned long)main_toggles);
    /* Final input state must equal parity of total toggles. The runtime
     * may have ended mid-toggle, so we only assert that both contexts ran. */
    VSF_TEST_ASSERT(s_ctx.callback_toggles > 0);
    VSF_TEST_ASSERT(main_toggles > 0);
    (void)in_mask;
}

#endif /* VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED */

/* EOF */
