#ifndef __VSF_TEST_RTC_ALARM_H__
#define __VSF_TEST_RTC_ALARM_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_ALARM_CASE_COUNT
#   define VSF_TEST_RTC_ALARM_CASE_COUNT       1
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
typedef struct {
    volatile bool alarm_triggered;
} vsf_test_rtc_alarm_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_rtc_alarm_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_RTC_ALARM_H__ */
/* EOF */
