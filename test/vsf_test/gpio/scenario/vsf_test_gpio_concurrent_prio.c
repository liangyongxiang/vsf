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

#include "vsf_test_gpio_concurrent_prio.h"
#include "RP2040.h"
#include "hardware/structs/timer.h"

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_concurrent_prio_case_t __gpio_concurrent_prio_cases[] = {
    VSF_TEST_GPIO_CONCURRENT_PRIO_CASES_INIT
};

/* Shared state between alarm0 ISR (high prio) and main loop (low prio). */
typedef struct {
    vsf_gpio_t          *gpio;
    vsf_gpio_pin_mask_t  out_mask;
    uint32_t             period_us;
    volatile uint32_t    callback_toggles;
    volatile bool        run;
} __concurrent_ctx_t;

static __concurrent_ctx_t s_ctx;

/* Hardware TIMER alarm 0 ISR — independent of the VSF kernel scheduler so
 * it fires even while vsf_test_run_tests is in its synchronous loop. */
void TIMER_IRQ_0_IRQHandler(void)
{
    /* Clear the alarm IRQ. */
    timer_hw->intr = (1u << 0);
    if (!s_ctx.run) { return; }
    vsf_gpio_toggle(s_ctx.gpio, s_ctx.out_mask);
    s_ctx.callback_toggles++;
    /* Re-arm. */
    timer_hw->alarm[0] = timer_hw->timerawl + s_ctx.period_us;
}

void vsf_test_gpio_concurrent_prio_add_cases(vsf_test_gpio_concurrent_prio_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][96];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_concurrent_prio_%u purpose=concurrency out=%u in=%u",
            (unsigned)__gpio_concurrent_prio_cases[i].idx,
            (unsigned)__gpio_concurrent_prio_cases[i].out_pin,
            (unsigned)__gpio_concurrent_prio_cases[i].in_pin);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_concurrent_prio_run,
            __cfg_str_pool[i], (void *)&__gpio_concurrent_prio_cases[i]);
        __gpio_concurrent_prio_cases[i].scene = scene;
    }
}

void vsf_test_gpio_concurrent_prio_run(const vsf_test_gpio_concurrent_prio_case_t *c)
{
    vsf_gpio_t *gpio = c->scene->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, out_mask);

    s_ctx.gpio = gpio;
    s_ctx.out_mask = out_mask;
    s_ctx.period_us = c->callback_period_us;
    s_ctx.callback_toggles = 0;
    s_ctx.run = true;

    /* Configure RP2040 hardware TIMER alarm 0. */
    timer_hw->intr = (1u << 0);       /* clear pending */
    timer_hw->inte = (1u << 0);       /* enable IRQ */
    timer_hw->alarm[0] = timer_hw->timerawl + c->callback_period_us;
    NVIC_SetPriority(TIMER_IRQ_0_IRQn, vsf_arch_prio_highest);
    NVIC_EnableIRQ(TIMER_IRQ_0_IRQn);

    uint32_t main_toggles = 0;
    /* Use vsf_test_busy_wait_ms to bound the duration — known-good and
     * doesn't rely on vsf_systimer advancing in tight loops. */
    uint32_t remaining_ms = c->duration_ms;
    while (remaining_ms > 0) {
        for (uint32_t i = 0; i < 1000; i++) {
            vsf_gpio_toggle(gpio, out_mask);
            main_toggles++;
        }
        vsf_test_busy_wait_ms(1);
        remaining_ms--;
    }

    s_ctx.run = false;
    NVIC_DisableIRQ(TIMER_IRQ_0_IRQn);
    timer_hw->inte = 0;

    vsf_trace_info("GPIO:CONCURRENT:cb=%lu main=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)s_ctx.callback_toggles, (unsigned long)main_toggles);
    /* Both contexts must have run. */
    VSF_TEST_ASSERT(s_ctx.callback_toggles > 0);
    VSF_TEST_ASSERT(main_toggles > 0);
}

#endif /* VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED */

/* EOF */
