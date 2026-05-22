/*============================ INCLUDES ======================================*/

#define __VSF_TEST_TIMER_CLASS_IMPLEMENT
#include "vsf_test_timer_periodic.h"

#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#define TIMER_PERIODIC_PERIOD_US               10000
#define TIMER_PERIODIC_COUNT                   5

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_timer_periodic_case_t __timer_periodic_cases[] = {
    VSF_TEST_TIMER_PERIODIC_CASES_INIT
};

/*============================ LOCAL FUNCTIONS ===============================*/

static void __timer_isr(void *target_ptr, vsf_timer_t *timer_ptr,
                        vsf_timer_irq_mask_t irq_mask)
{
    (void)timer_ptr;
    vsf_test_timer_periodic_suite_t *suite = (vsf_test_timer_periodic_suite_t *)target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        if (suite->counter < TIMER_PERIODIC_COUNT) {
            suite->counter++;
        }
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_timer_periodic_add_cases(vsf_test_timer_periodic_suite_t *suite)
{
    suite->name    = "timer_periodic";
    suite->purpose = "timer_periodic";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_TIMER_PERIODIC_CASE_COUNT; i++) {
        __timer_periodic_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_timer_periodic_run,
            (void *)&__timer_periodic_cases[i]);
    }
}

void vsf_test_timer_periodic_run(void *arg)
{
    vsf_test_timer_periodic_case_t *c = (vsf_test_timer_periodic_case_t *)arg;
    vsf_timer_t *timer = c->suite->timer;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    c->suite->counter = 0;

    vsf_timer_capability_t cap = vsf_timer_capability(timer);
    VSF_TEST_ASSERT(cap.channel_cnt >= 1);
    VSF_TEST_ASSERT(cap.timer_bitlen == 32);

    vsf_err_t err = vsf_timer_init(timer, &(vsf_timer_cfg_t){
        .period = TIMER_PERIODIC_PERIOD_US,
        .isr = {
            .handler_fn = __timer_isr,
            .target_ptr = c->suite,
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
    while (c->suite->counter < TIMER_PERIODIC_COUNT && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }

    VSF_TEST_ASSERT(c->suite->counter == TIMER_PERIODIC_COUNT);

    vsf_trace_info("TIMER:PERIODIC:PASS" VSF_TRACE_CFG_LINEEND);

    vsf_timer_channel_stop(timer, 0);
    while (fsm_rt_cpl != vsf_timer_disable(timer));
    vsf_timer_fini(timer);
}

#endif /* VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED */

/* EOF */
