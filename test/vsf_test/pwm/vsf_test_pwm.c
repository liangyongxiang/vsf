/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm.h"
#include "scenario/vsf_test_pwm_basic.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware scenarios: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
void vsf_test_pwm_register_all(vsf_test_pwm_suites_t *s, vsf_pwm_t *pwm)
{
    s->basic.pwm = pwm;
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    vsf_test_pwm_basic_add_cases(&s->basic);
#endif
}

/* EOF */
