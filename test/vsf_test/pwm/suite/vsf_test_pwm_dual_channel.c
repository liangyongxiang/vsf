/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm_dual_channel.h"
#include "vsf_test_suites.h"

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_pwm_dual_channel_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_pwm_dual_channel_params_t *p = tc->arg;
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

    /* Channel A: 50% duty (period=100, pulse=50) */
    err = vsf_pwm_set(pwm, 0, 100, 50);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Channel B: 25% duty (period=100, pulse=25) */
    err = vsf_pwm_set(pwm, 1, 100, 25);
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

    vsf_trace_info("PWM:DUAL_CHANNEL:PASS" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED */

/* EOF */
