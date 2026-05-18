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

typedef struct vsf_test_pwm_basic_scene_t {
    vsf_pwm_t *pwm;
} vsf_test_pwm_basic_scene_t;

typedef struct vsf_test_pwm_scenes_t {
    vsf_test_pwm_basic_scene_t basic;
} vsf_test_pwm_scenes_t;

/*============================ PROTOTYPES ====================================*/

void vsf_test_pwm_register_all(vsf_test_pwm_scenes_t *s);

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
void vsf_test_pwm_basic_add_cases(vsf_test_pwm_basic_scene_t *scene);
void vsf_test_pwm_basic_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_PWM_H__ */
/* EOF */
