#ifndef __VSF_TEST_PWM_H__
#define __VSF_TEST_PWM_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_BASIC_ENABLE
#   define VSF_TEST_PWM_BASIC_ENABLE           ENABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_pwm_basic_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_pwm_t *pwm;
    )
};

typedef struct vsf_test_pwm_suites_t {
    vsf_test_pwm_basic_suite_t basic;
} vsf_test_pwm_suites_t;

/*============================ PROTOTYPES ====================================*/

void vsf_test_pwm_register_all(vsf_test_pwm_suites_t *s, vsf_pwm_t *pwm);

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
void vsf_test_pwm_basic_add_cases(vsf_test_pwm_basic_suite_t *suite);
void vsf_test_pwm_basic_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_PWM_H__ */
/* EOF */
