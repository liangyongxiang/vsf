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

#ifndef __VSF_TEST_RTC_H__
#define __VSF_TEST_RTC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#if     defined(__VSF_TEST_RTC_CLASS_IMPLEMENT)
#   undef __VSF_TEST_RTC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_SET_GET_ENABLE
#   define VSF_TEST_RTC_SET_GET_ENABLE         ENABLED
#endif

#ifndef VSF_TEST_RTC_ALARM_ENABLE
#   define VSF_TEST_RTC_ALARM_ENABLE           ENABLED
#endif

#ifndef VSF_TEST_RTC_EPOCH_ENABLE
#   define VSF_TEST_RTC_EPOCH_ENABLE            ENABLED
#endif

/*============================ TYPES =========================================*/





#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
vsf_class(vsf_test_rtc_set_get_params_t) {
    public_member(
        uint8_t idx;
        uint8_t rtc_idx;
    )
};
#endif

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
vsf_class(vsf_test_rtc_alarm_params_t) {
    public_member(
        uint8_t idx;
        uint8_t rtc_idx;
    )
};
#endif

#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
vsf_class(vsf_test_rtc_epoch_params_t) {
    public_member(
        uint8_t idx;
        uint8_t rtc_idx;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
void vsf_test_rtc_set_get_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
void vsf_test_rtc_alarm_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
void vsf_test_rtc_epoch_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

// Framework types — included LAST so this header can be pulled into
// vsf_test.h (which needs vsf_test_rtc_suites_t) without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif


/*============================ SUITE TABLE ==================================*/

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
#   define __vsf_test_rtc_alarm_suite { .name = "rtc_alarm", .cases = __rtc_alarm_cases, .case_count = dimof(__rtc_alarm_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_RTC },
#else
#   define __vsf_test_rtc_alarm_suite
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
#   define __vsf_test_rtc_epoch_suite { .name = "rtc_epoch", .cases = __rtc_epoch_cases, .case_count = dimof(__rtc_epoch_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_RTC },
#else
#   define __vsf_test_rtc_epoch_suite
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
#   define __vsf_test_rtc_set_get_suite { .name = "rtc_set_get", .cases = __rtc_set_get_cases, .case_count = dimof(__rtc_set_get_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_RTC },
#else
#   define __vsf_test_rtc_set_get_suite
#endif

#define VSF_TEST_RTC_SUITES \
    __vsf_test_rtc_alarm_suite \
    __vsf_test_rtc_epoch_suite \
    __vsf_test_rtc_set_get_suite

#endif /* __VSF_TEST_RTC_H__ */
/* EOF */
