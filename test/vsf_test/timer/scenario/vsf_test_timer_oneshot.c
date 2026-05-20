/*============================ INCLUDES ======================================*/

#define __VSF_TEST_TIMER_CLASS_IMPLEMENT
#include "vsf_test_timer_oneshot.h"

#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS            200
#endif

#define TIMER_ONESHOT_PERIOD_US                50000

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_timer_oneshot_case_t __timer_oneshot_cases[] = {
    VSF_TEST_TIMER_ONESHOT_CASES_INIT
};

/*============================ LOCAL FUNCTIONS ===============================*/

static void __timer_isr(void *target_ptr, vsf_timer_t *timer_ptr,
                        vsf_timer_irq_mask_t irq_mask)
{
    (void)timer_ptr;
    vsf_test_timer_oneshot_scene_t *scene = (vsf_test_timer_oneshot_scene_t *)target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        scene->fired = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_timer_oneshot_add_cases(vsf_test_timer_oneshot_scene_t *scene)
{
    scene->name    = "timer_oneshot";
    scene->purpose = "timer_oneshot";
    scene->hw_req  = "none";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_TIMER_ONESHOT_CASE_COUNT; i++) {
        __timer_oneshot_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_timer_oneshot_run,
            (void *)&__timer_oneshot_cases[i]);
    }
}

void vsf_test_timer_oneshot_run(void *arg)
{
    vsf_test_timer_oneshot_case_t *c = (vsf_test_timer_oneshot_case_t *)arg;
    vsf_timer_t *timer = c->scene->timer;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    c->scene->fired = false;

    vsf_timer_capability_t cap = vsf_timer_capability(timer);
    VSF_TEST_ASSERT(cap.channel_cnt >= 1);
    VSF_TEST_ASSERT(cap.timer_bitlen == 32);

    vsf_err_t err = vsf_timer_init(timer, &(vsf_timer_cfg_t){
        .period = TIMER_ONESHOT_PERIOD_US,
        .isr = {
            .handler_fn = __timer_isr,
            .target_ptr = c->scene,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_timer_enable(timer));

    /* Configure channel 0 for one-shot mode */
    err = vsf_timer_channel_config(timer, 0, &(vsf_timer_channel_cfg_t){
        .mode  = VSF_TIMER_CHANNEL_MODE_BASE | VSF_TIMER_BASE_ONESHOT,
        .pulse = TIMER_ONESHOT_PERIOD_US,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Start the channel alarm */
    err = vsf_timer_channel_start(timer, 0);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Wait up to ~150ms for the alarm to fire */
    uint32_t timeout_ms = 150;
    while (!c->scene->fired && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }

    VSF_TEST_ASSERT(c->scene->fired);

    vsf_trace_info("TIMER:ONESHOT:PASS" VSF_TRACE_CFG_LINEEND);

    vsf_timer_channel_stop(timer, 0);
    while (fsm_rt_cpl != vsf_timer_disable(timer));
    vsf_timer_fini(timer);
}

#endif /* VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED */

/* EOF */
