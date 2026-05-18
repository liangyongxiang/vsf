/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm.h"
#include "scenario/vsf_test_pwm_basic.h"

/*============================ IMPLEMENTATION ================================*/

#define REG_IF(gate, s, field, add_fn)            \
    VSF_TEST_REGISTER_SCENE(s, field, add_fn)

void vsf_test_pwm_register_all(vsf_test_pwm_scenes_t *s)
{
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    REG_IF("pwm_basic", s, basic, vsf_test_pwm_basic_add_cases);
#endif
}

/* EOF */
