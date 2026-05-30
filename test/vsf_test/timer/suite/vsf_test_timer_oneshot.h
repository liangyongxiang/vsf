#ifndef __VSF_TEST_TIMER_ONESHOT_H__
#define __VSF_TEST_TIMER_ONESHOT_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_timer.h"

#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
typedef struct {
    volatile bool fired;
} vsf_test_timer_oneshot_var_t;
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ONESHOT_CASE_COUNT
#   define VSF_TEST_TIMER_ONESHOT_CASE_COUNT   1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_timer_oneshot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_TIMER_ONESHOT_H__ */
/* EOF */
