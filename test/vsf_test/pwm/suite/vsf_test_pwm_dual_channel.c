/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm_dual_channel.h"

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_pwm_dual_channel_case_t __pwm_dual_channel_cases[] = {
    VSF_TEST_PWM_DUAL_CHANNEL_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_pwm_dual_channel_add_cases(vsf_test_pwm_dual_channel_suite_t *suite)
{
    suite->name    = "pwm_dual_channel";
    suite->purpose = "pwm_dual_channel";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT; i++) {
        __pwm_dual_channel_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_pwm_dual_channel_run,
            (void *)&__pwm_dual_channel_cases[i]);
    }
}

void vsf_test_pwm_dual_channel_run(void *arg)
{
    vsf_test_pwm_dual_channel_case_t *c = (vsf_test_pwm_dual_channel_case_t *)arg;
    vsf_pwm_t *pwm = c->suite->pwm;

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
