#ifndef __TEST_WDT_REBOOT_H__
#define __TEST_WDT_REBOOT_H__

#include "../vsf_test_wdt.h"

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED

#ifndef VSF_TEST_WDT_REBOOT_CASE_COUNT
#   define VSF_TEST_WDT_REBOOT_CASE_COUNT      1
#endif

#define VSF_TEST_WDT_REBOOT_CASES_INIT                                      \
    { 0 }

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vsf_test_wdt_reboot_case_t {
    uint8_t idx;
    vsf_test_wdt_reboot_suite_t *suite;
} vsf_test_wdt_reboot_case_t;

void vsf_test_wdt_reboot_add_cases(vsf_test_wdt_reboot_suite_t *suite);
void vsf_test_wdt_reboot_run(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_WDT_REBOOT_ENABLE == ENABLED */

#endif /* __TEST_WDT_REBOOT_H__ */
/* EOF */
