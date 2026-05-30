/*============================ INCLUDES ======================================*/

#define __VSF_TEST_TIMER_CLASS_IMPLEMENT
#include "vsf_test_timer_async.h"
#include "vsf_test_suites.h"
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED

static void __timer_isr(void *target_ptr, vsf_timer_t *timer_ptr,
                        vsf_timer_irq_mask_t irq_mask)
{
    (void)timer_ptr;
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        if (vsf_test_suites.timer_async.counter < 255) {
            vsf_test_suites.timer_async.counter++;
        }
    }
}



/*============================ MACROS ========================================*/

#define TIMER_ASYNC_PERIOD_US     10000
#define TIMER_ASYNC_COUNT         10

/*============================ IMPLEMENTATION ================================*/

void vsf_test_timer_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_timer_async_params_t *p = tc->arg;
    vsf_timer_t *timer = (vsf_timer_t *)fixture;

    vsf_test_suites.timer_async.counter = 0;

    vsf_timer_capability_t cap = vsf_timer_capability(timer);
    VSF_TEST_ASSERT(cap.channel_cnt >= 1);
    VSF_TEST_ASSERT(cap.timer_bitlen == 32);

    vsf_err_t err = vsf_timer_init(timer, &(vsf_timer_cfg_t){
        .period = TIMER_ASYNC_PERIOD_US,
        .isr = {
            .handler_fn = __timer_isr,
            .target_ptr = NULL,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_timer_enable(timer));

    /* --- Test 1: Async oneshot via channel_request_start --- */
    vsf_test_suites.timer_async.counter = 0;
    uint32_t period_buf = TIMER_ASYNC_PERIOD_US;
    err = vsf_timer_channel_request_start(timer, 0, &(vsf_timer_channel_request_t){
        .length = 1,
        .period_buffer = &period_buf,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* status should report busy while channel is running */
    vsf_timer_status_t status = vsf_timer_status(timer);
    VSF_TEST_ASSERT(status.value != 0);

    uint32_t timeout_ms = 150;
    while (vsf_test_suites.timer_async.counter < 1 && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }
    VSF_TEST_ASSERT(vsf_test_suites.timer_async.counter == 1);

    /* irq_clear should return overflow mask after alarm fired, then 0 */
    vsf_timer_irq_mask_t cleared = vsf_timer_irq_clear(timer, VSF_TIMER_IRQ_MASK_OVERFLOW);
    VSF_TEST_ASSERT(cleared == VSF_TIMER_IRQ_MASK_OVERFLOW);
    cleared = vsf_timer_irq_clear(timer, VSF_TIMER_IRQ_MASK_OVERFLOW);
    VSF_TEST_ASSERT(cleared == 0);

    /* status should be idle after oneshot completes */
    status = vsf_timer_status(timer);
    VSF_TEST_ASSERT(status.value == 0);

    /* --- Test 2: Async periodic via channel_request_start --- */
    vsf_test_suites.timer_async.counter = 0;
    err = vsf_timer_channel_config(timer, 0, &(vsf_timer_channel_cfg_t){
        .mode  = VSF_TIMER_CHANNEL_MODE_BASE | VSF_TIMER_BASE_CONTINUES,
        .pulse = TIMER_ASYNC_PERIOD_US,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_timer_channel_request_start(timer, 0, &(vsf_timer_channel_request_t){
        .length = 1,
        .period_buffer = &period_buf,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    timeout_ms = 200;
    while (vsf_test_suites.timer_async.counter < TIMER_ASYNC_COUNT && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }
    VSF_TEST_ASSERT(vsf_test_suites.timer_async.counter == TIMER_ASYNC_COUNT);

    /* --- Test 3: Async stop via channel_request_stop --- */
    vsf_test_suites.timer_async.counter = 0;
    err = vsf_timer_channel_request_start(timer, 0, &(vsf_timer_channel_request_t){
        .length = 1,
        .period_buffer = &period_buf,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    timeout_ms = 50;
    while (vsf_test_suites.timer_async.counter < 3 && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }
    VSF_TEST_ASSERT(vsf_test_suites.timer_async.counter >= 3);

    err = vsf_timer_channel_request_stop(timer, 0);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    uint8_t count_after_stop = vsf_test_suites.timer_async.counter;
    vsf_test_busy_wait_ms(50);
    VSF_TEST_ASSERT(vsf_test_suites.timer_async.counter == count_after_stop);

    vsf_trace_info("TIMER:ASYNC:PASS" VSF_TRACE_CFG_LINEEND);

    vsf_timer_channel_stop(timer, 0);
    while (fsm_rt_cpl != vsf_timer_disable(timer));
    vsf_timer_fini(timer);
}

#endif /* VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED */

/* EOF */