#ifndef __VSF_TEST_TIMER_ONESHOT_H__
#define __VSF_TEST_TIMER_ONESHOT_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_timer.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ONESHOT_CASE_COUNT
#   define VSF_TEST_TIMER_ONESHOT_CASE_COUNT   1
#endif

#define VSF_TEST_TIMER_ONESHOT_CASES_INIT                                   \
    { 0 }

/*============================ TYPES =========================================*/

typedef struct vsf_test_timer_oneshot_case_t {
    uint8_t idx;
    vsf_test_timer_oneshot_scene_t *scene;
} vsf_test_timer_oneshot_case_t;

#endif /* __VSF_TEST_TIMER_ONESHOT_H__ */
/* EOF */
