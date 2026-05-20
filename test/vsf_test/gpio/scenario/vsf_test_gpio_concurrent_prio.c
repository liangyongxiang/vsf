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
#include "RP2040.h"
#include "hardware/structs/timer.h"

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_concurrent_prio_case_t __gpio_concurrent_prio_cases[] = {
    VSF_TEST_GPIO_CONCURRENT_PRIO_CASES_INIT
};

/* TIMER_IRQ_0_IRQHandler is the chip's bare-metal vector — no target_ptr.
 * We bind the active scene here so the ISR can route through it. run() sets
 * this before enabling the IRQ and clears it after disabling. */
static vsf_test_gpio_concurrent_prio_scene_t *s_active_scene;

/* Hardware TIMER alarm 0 ISR — independent of the VSF kernel scheduler so
 * it fires even while vsf_test_run_tests is in its synchronous loop. */
void TIMER_IRQ_0_IRQHandler(void)
{
    /* Clear the alarm IRQ. */
    timer_hw->intr = (1u << 0);
    vsf_test_gpio_concurrent_prio_scene_t *scene = s_active_scene;
    if (scene == NULL || !scene->run) { return; }
    vsf_gpio_toggle(scene->gpio, scene->out_mask);
    scene->callback_toggles++;
    /* Re-arm. */
    timer_hw->alarm[0] = timer_hw->timerawl + scene->period_us;
}

void vsf_test_gpio_concurrent_prio_add_cases(vsf_test_gpio_concurrent_prio_scene_t *scene)
{
    scene->name    = "gpio_concurrent_prio";
    scene->purpose = "concurrency";
    scene->hw_req  = "none";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT; i++) {
        __gpio_concurrent_prio_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_concurrent_prio_run,
            (void *)&__gpio_concurrent_prio_cases[i]);
    }
}

void vsf_test_gpio_concurrent_prio_run(const vsf_test_gpio_concurrent_prio_case_t *c)
{
    vsf_gpio_t *gpio = c->scene->gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << c->out_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, out_mask);

    /* Per-case state in scene: must be re-initialised before each run. */
    c->scene->out_mask         = out_mask;
    c->scene->period_us        = c->callback_period_us;
    c->scene->callback_toggles = 0;
    c->scene->main_toggles     = 0;
    c->scene->run              = true;
    s_active_scene = c->scene;

    /* Configure RP2040 hardware TIMER alarm 0. */
    timer_hw->intr = (1u << 0);       /* clear pending */
    timer_hw->inte = (1u << 0);       /* enable IRQ */
    timer_hw->alarm[0] = timer_hw->timerawl + c->callback_period_us;
    NVIC_SetPriority(TIMER_IRQ_0_IRQn, vsf_arch_prio_highest);
    NVIC_EnableIRQ(TIMER_IRQ_0_IRQn);

    /* Use vsf_test_busy_wait_ms to bound the duration — known-good and
     * doesn't rely on vsf_systimer advancing in tight loops. */
    uint32_t remaining_ms = c->duration_ms;
    while (remaining_ms > 0) {
        for (uint32_t i = 0; i < 1000; i++) {
            vsf_gpio_toggle(gpio, out_mask);
            c->scene->main_toggles++;
        }
        vsf_test_busy_wait_ms(1);
        remaining_ms--;
    }

    c->scene->run = false;
    NVIC_DisableIRQ(TIMER_IRQ_0_IRQn);
    timer_hw->inte = 0;
    s_active_scene = NULL;

    vsf_trace_info("GPIO:CONCURRENT:cb=%lu main=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)c->scene->callback_toggles,
                   (unsigned long)c->scene->main_toggles);
    /* Both contexts must have run. */
    VSF_TEST_ASSERT(c->scene->callback_toggles > 0);
    VSF_TEST_ASSERT(c->scene->main_toggles > 0);
}

#endif /* VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED */

/* EOF */
