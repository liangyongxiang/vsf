/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  You may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __VSF_TEST_TIMER_H__
#define __VSF_TEST_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#if     defined(__VSF_TEST_TIMER_CLASS_IMPLEMENT)
#   undef __VSF_TEST_TIMER_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ONESHOT_ENABLE
#   define VSF_TEST_TIMER_ONESHOT_ENABLE         ENABLED
#endif

#ifndef VSF_TEST_TIMER_PERIODIC_ENABLE
#   define VSF_TEST_TIMER_PERIODIC_ENABLE        ENABLED
#endif

#ifndef VSF_TEST_TIMER_ASYNC_ENABLE
#   define VSF_TEST_TIMER_ASYNC_ENABLE           ENABLED
#endif

/*============================ TYPES =========================================*/




#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
vsf_class(vsf_test_timer_oneshot_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  timer_idx;
        uint8_t  channel;
        uint32_t period_us;
    )
};
#endif

#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
vsf_class(vsf_test_timer_periodic_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  timer_idx;
        uint8_t  channel;
        uint32_t period_us;
        uint8_t  count;
    )
};
#endif

#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED

vsf_class(vsf_test_timer_async_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  timer_idx;
        uint8_t  channel;
        uint32_t period_us;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
void vsf_test_timer_oneshot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
void vsf_test_timer_periodic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
void vsf_test_timer_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

// Framework types — included LAST so this header can be pulled into
// vsf_test.h (which needs vsf_test_timer_suites_t) without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif


/*============================ SUITE TABLE ==================================*/

#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
#   define __vsf_test_timer_async_suite { .name = "timer_async", .cases = __timer_async_cases, .case_count = dimof(__timer_async_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER },
#else
#   define __vsf_test_timer_async_suite
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
#   define __vsf_test_timer_oneshot_suite { .name = "timer_oneshot", .cases = __timer_oneshot_cases, .case_count = dimof(__timer_oneshot_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER },
#else
#   define __vsf_test_timer_oneshot_suite
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
#   define __vsf_test_timer_periodic_suite { .name = "timer_periodic", .cases = __timer_periodic_cases, .case_count = dimof(__timer_periodic_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER },
#else
#   define __vsf_test_timer_periodic_suite
#endif

#define VSF_TEST_TIMER_SUITES \
    __vsf_test_timer_async_suite \
    __vsf_test_timer_oneshot_suite \
    __vsf_test_timer_periodic_suite

#endif /* __VSF_TEST_TIMER_H__ */
/* EOF */
