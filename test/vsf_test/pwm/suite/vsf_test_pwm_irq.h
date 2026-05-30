#ifndef __VSF_TEST_PWM_IRQ_H__
#define __VSF_TEST_PWM_IRQ_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_pwm.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_IRQ_CASE_COUNT
#   define VSF_TEST_PWM_IRQ_CASE_COUNT       1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_pwm_irq_run(vsf_test_case_t *tc);

#endif /* __VSF_TEST_PWM_IRQ_H__ */
/* EOF */
