#ifndef __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__
#define __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_gpio.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT     3
#endif

#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_CASES_INIT
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_CASES_INIT     \
        { .idx = 0, .pin = 4, .interval_ms = 10,  .toggle_count = 10 }, \
        { .idx = 1, .pin = 4, .interval_ms = 50,  .toggle_count = 10 }, \
        { .idx = 2, .pin = 4, .interval_ms = 100, .toggle_count = 5  }
#endif

#endif /* __VSF_TEST_GPIO_SYSTIMER_HEALTH_H__ */
/* EOF */
