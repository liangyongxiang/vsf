#ifndef __VSF_TEST_PWM_IRQ_H__
#define __VSF_TEST_PWM_IRQ_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_pwm.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_IRQ_CASE_COUNT
#   define VSF_TEST_PWM_IRQ_CASE_COUNT       1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_pwm_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_PWM_IRQ_H__ */
/* EOF */
