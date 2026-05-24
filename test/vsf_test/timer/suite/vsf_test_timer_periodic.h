#ifndef __VSF_TEST_TIMER_PERIODIC_H__
#define __VSF_TEST_TIMER_PERIODIC_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_timer.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_PERIODIC_CASE_COUNT
#   define VSF_TEST_TIMER_PERIODIC_CASE_COUNT   1
#endif

#define VSF_TEST_TIMER_PERIODIC_CASES_INIT                                   \
    { 0 }

/*============================ TYPES =========================================*/

typedef struct vsf_test_timer_periodic_case_t {
    uint8_t idx;
    vsf_test_timer_periodic_suite_t *suite;
} vsf_test_timer_periodic_case_t;

#endif /* __VSF_TEST_TIMER_PERIODIC_H__ */
/* EOF */
