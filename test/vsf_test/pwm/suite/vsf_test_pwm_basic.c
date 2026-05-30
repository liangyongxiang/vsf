/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm_basic.h"

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ IMPLEMENTATION ================================*/

void vsf_test_pwm_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_pwm_basic_params_t *p = tc->arg;
    vsf_pwm_t *pwm = (vsf_pwm_t *)fixture;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    /* Test capability */
    vsf_pwm_capability_t cap = vsf_pwm_capability(pwm);
    VSF_TEST_ASSERT(cap.max_freq > 0);
    VSF_TEST_ASSERT(cap.min_freq > 0);

    /* Initialize PWM at 1 kHz */
    vsf_err_t err = vsf_pwm_init(pwm, &(vsf_pwm_cfg_t){
        .freq = 1000,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Set 50% duty: period=100 counts, pulse=50 counts */
    err = vsf_pwm_set(pwm, 0, 100, 50);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Enable PWM output */
    while (fsm_rt_cpl != vsf_pwm_enable(pwm));

    /* Let it run for a short time */
    vsf_test_busy_wait_ms(10);

    /* Verify get_freq returns non-zero */
    uint32_t freq = vsf_pwm_get_freq(pwm);
    VSF_TEST_ASSERT(freq > 0);

    /* Disable PWM */
    while (fsm_rt_cpl != vsf_pwm_disable(pwm));

    vsf_trace_info("PWM:BASIC:PASS" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_PWM_BASIC_ENABLE == ENABLED */

/* EOF */
