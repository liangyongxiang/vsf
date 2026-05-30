#ifndef __TEST_WDT_REBOOT_H__
#define __TEST_WDT_REBOOT_H__

#include "../vsf_test_wdt.h"

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED

#ifndef VSF_TEST_WDT_REBOOT_CASE_COUNT
#   define VSF_TEST_WDT_REBOOT_CASE_COUNT      1
#endif

#ifdef __cplusplus
extern "C" {
#endif

void vsf_test_wdt_reboot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_WDT_REBOOT_ENABLE == ENABLED */

#endif /* __TEST_WDT_REBOOT_H__ */
/* EOF */
