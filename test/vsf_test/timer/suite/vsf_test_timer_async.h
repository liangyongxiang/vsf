#ifndef __VSF_TEST_TIMER_ASYNC_H__
#define __VSF_TEST_TIMER_ASYNC_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_timer.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ASYNC_CASE_COUNT
#   define VSF_TEST_TIMER_ASYNC_CASE_COUNT   1
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
typedef struct {
    volatile uint8_t counter;
} vsf_test_timer_async_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_timer_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_TIMER_ASYNC_H__ */
/* EOF */
