#ifndef __VSF_TEST_RTC_EPOCH_H__
#define __VSF_TEST_RTC_EPOCH_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_rtc.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_EPOCH_CASE_COUNT
#   define VSF_TEST_RTC_EPOCH_CASE_COUNT     1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_rtc_epoch_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_RTC_EPOCH_H__ */
/* EOF */
