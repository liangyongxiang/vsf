#ifndef __VSF_TEST_PWM_DUAL_CHANNEL_H__
#define __VSF_TEST_PWM_DUAL_CHANNEL_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_pwm.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT
#   define VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT       1
#endif

#define VSF_TEST_PWM_DUAL_CHANNEL_CASES_INIT                                       \
    { 0 }

/*============================ TYPES =========================================*/

typedef struct vsf_test_pwm_dual_channel_case_t {
    uint8_t idx;
    vsf_test_pwm_dual_channel_suite_t *suite;
} vsf_test_pwm_dual_channel_case_t;

#endif /* __VSF_TEST_PWM_DUAL_CHANNEL_H__ */
/* EOF */
