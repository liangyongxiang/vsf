#ifndef __VSF_TEST_TIMER_ASYNC_H__
#define __VSF_TEST_TIMER_ASYNC_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_timer.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ASYNC_CASE_COUNT
#   define VSF_TEST_TIMER_ASYNC_CASE_COUNT   1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_timer_async_run(vsf_test_case_t *tc);

#endif /* __VSF_TEST_TIMER_ASYNC_H__ */
/* EOF */
