/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm_basic.h"

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_pwm_basic_case_t __pwm_basic_cases[] = {
    VSF_TEST_PWM_BASIC_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_pwm_basic_add_cases(vsf_test_pwm_basic_suite_t *suite)
{
    suite->name    = "pwm_basic";
    suite->purpose = "pwm_basic";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_PWM_BASIC_CASE_COUNT; i++) {
        __pwm_basic_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_pwm_basic_run,
            (void *)&__pwm_basic_cases[i]);
    }
}

void vsf_test_pwm_basic_run(void *arg)
{
    vsf_test_pwm_basic_case_t *c = (vsf_test_pwm_basic_case_t *)arg;
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
