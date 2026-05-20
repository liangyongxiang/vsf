#ifndef __VSF_TEST_RTC_ALARM_H__
#define __VSF_TEST_RTC_ALARM_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_rtc.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_ALARM_CASE_COUNT
#   define VSF_TEST_RTC_ALARM_CASE_COUNT       1
#endif

#define VSF_TEST_RTC_ALARM_CASES_INIT                                       \
    { 0 }

/*============================ TYPES =========================================*/

typedef struct vsf_test_rtc_alarm_case_t {
    uint8_t idx;
    vsf_test_rtc_alarm_suite_t *suite;
} vsf_test_rtc_alarm_case_t;

#endif /* __VSF_TEST_RTC_ALARM_H__ */
/* EOF */
