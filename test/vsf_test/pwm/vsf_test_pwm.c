/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm.h"
#include "scenario/vsf_test_pwm_basic.h"

/*============================ IMPLEMENTATION ================================*/

#define REG_IF(gate, s, field, add_fn)            \
    do {                                          \
        vsf_test_shell_register_scene(vsf_test_get_shell(), gate); \
        add_fn(&(s)->field);                      \
    } while (0)

void vsf_test_pwm_register_all(vsf_test_pwm_scenes_t *s)
{
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    REG_IF("pwm_basic", s, basic, vsf_test_pwm_basic_add_cases);
#endif
}

/* EOF */
