/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm.h"
#include "suite/vsf_test_pwm_basic.h"
#include "suite/vsf_test_pwm_dual_channel.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
vsf_test_pwm_suites_t vsf_test_pwm_suites;

void vsf_test_pwm_init(vsf_test_pwm_suites_t *s,
                         const vsf_test_pwm_suite_binding_t bindings[],
                         uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        vsf_test_pwm_suite_base_t *suite = bindings[i].suite;
        vsf_pwm_t                *inst  = bindings[i].instance;
        if (inst == NULL) { continue; }

        suite->pwm  = inst;
        suite->setup  = bindings[i].setup;
        suite->teardown = bindings[i].teardown;
    }
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    vsf_test_pwm_basic_add_cases(&s->basic);
#endif

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    vsf_test_pwm_dual_channel_add_cases(&s->dual_channel);
#endif

}


/* EOF */
