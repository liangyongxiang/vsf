/*============================ INCLUDES ======================================*/

#define __VSF_TEST_TIMER_CLASS_IMPLEMENT
#include "vsf_test_timer_oneshot.h"
/*============================ LOCAL VARIABLES ===============================*/

static volatile bool __fired;

static void __timer_isr(void *target_ptr, vsf_timer_t *timer_ptr,
                        vsf_timer_irq_mask_t irq_mask)
{
    (void)timer_ptr;
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        __fired = true;
    }
}



#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED

/*============================ MACROS ========================================*/


#define TIMER_ONESHOT_PERIOD_US                50000

/*============================ IMPLEMENTATION ================================*/

void vsf_test_timer_oneshot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_timer_oneshot_params_t *p = tc->arg;
    vsf_timer_t *timer = (vsf_timer_t *)fixture;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    __fired = false;

    vsf_timer_capability_t cap = vsf_timer_capability(timer);
    VSF_TEST_ASSERT(cap.channel_cnt >= 1);
    VSF_TEST_ASSERT(cap.timer_bitlen == 32);

    vsf_err_t err = vsf_timer_init(timer, &(vsf_timer_cfg_t){
        .period = TIMER_ONESHOT_PERIOD_US,
        .isr = {
            .handler_fn = __timer_isr,
            .target_ptr = NULL,
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
    while (!__fired && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }

    VSF_TEST_ASSERT(__fired);

    vsf_trace_info("TIMER:ONESHOT:PASS" VSF_TRACE_CFG_LINEEND);

    vsf_timer_channel_stop(timer, 0);
    while (fsm_rt_cpl != vsf_timer_disable(timer));
    vsf_timer_fini(timer);
}

#endif /* VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED */

/* EOF */
