/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm.h"
#include "suite/vsf_test_pwm_basic.h"
#include "suite/vsf_test_pwm_dual_channel.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
void vsf_test_pwm_init(vsf_test_pwm_suites_t *s, const vsf_test_pwm_cfg_t *cfg)
{
    s->basic.pwm = cfg->pwm;
    s->dual_channel.pwm = cfg->pwm;
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    vsf_test_pwm_basic_add_cases(&s->basic);
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    vsf_test_pwm_dual_channel_add_cases(&s->dual_channel);
#endif
}

/* EOF */
