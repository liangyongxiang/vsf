#ifndef __VSF_TEST_RTC_SET_GET_H__
#define __VSF_TEST_RTC_SET_GET_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_SET_GET_CASE_COUNT
#   define VSF_TEST_RTC_SET_GET_CASE_COUNT     1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_rtc_set_get_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_RTC_SET_GET_H__ */
/* EOF */
