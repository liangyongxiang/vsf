#ifndef __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__
#define __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_gpio.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT     1
#endif

#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_CASES_INIT
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_CASES_INIT     \
        { .idx = 0, .pin = 4, .interval_ms = 10,  .toggle_count = 10 }
#endif

#endif /* __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__ */
/* EOF */
