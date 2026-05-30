/*============================ INCLUDES ======================================*/

#define __VSF_TEST_TIMER_CLASS_IMPLEMENT
#include "vsf_test_timer_periodic.h"
#include "vsf_test_suites.h"
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#define TIMER_PERIODIC_PERIOD_US               10000
#define TIMER_PERIODIC_COUNT                   5

/*============================ LOCAL FUNCTIONS ===============================*/

static void __timer_isr(void *target_ptr, vsf_timer_t *timer_ptr,
                        vsf_timer_irq_mask_t irq_mask)
{
    (void)timer_ptr;
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        if (vsf_test_suite_data.timer_periodic.counter < TIMER_PERIODIC_COUNT) {
            vsf_test_suite_data.timer_periodic.counter++;
        }
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_timer_periodic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_timer_periodic_params_t *p = tc->arg;
    vsf_timer_t *timer = (vsf_timer_t *)fixture;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    vsf_test_suite_data.timer_periodic.counter = 0;

    vsf_timer_capability_t cap = vsf_timer_capability(timer);
    VSF_TEST_ASSERT(cap.channel_cnt >= 1);
    VSF_TEST_ASSERT(cap.timer_bitlen == 32);

    vsf_err_t err = vsf_timer_init(timer, &(vsf_timer_cfg_t){
        .period = TIMER_PERIODIC_PERIOD_US,
        .isr = {
            .handler_fn = __timer_isr,
            .target_ptr = NULL,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_timer_enable(timer));

    /* Configure channel 0 for periodic mode */
    err = vsf_timer_channel_config(timer, 0, &(vsf_timer_channel_cfg_t){
        .mode  = VSF_TIMER_CHANNEL_MODE_BASE | VSF_TIMER_BASE_CONTINUES,
        .pulse = TIMER_PERIODIC_PERIOD_US,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Start the periodic channel */
    err = vsf_timer_channel_start(timer, 0);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Wait up to ~200ms for all 5 periodic interrupts to fire */
    uint32_t timeout_ms = 200;
    while (vsf_test_suite_data.timer_periodic.counter < TIMER_PERIODIC_COUNT && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }

    VSF_TEST_ASSERT(vsf_test_suite_data.timer_periodic.counter == TIMER_PERIODIC_COUNT);

    vsf_trace_info("TIMER:PERIODIC:PASS" VSF_TRACE_CFG_LINEEND);

    vsf_timer_channel_stop(timer, 0);
    while (fsm_rt_cpl != vsf_timer_disable(timer));
    vsf_timer_fini(timer);
}

#endif /* VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED */

/* EOF */