#ifndef __VSF_TEST_WDT_H__
#define __VSF_TEST_WDT_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_WDT_BASIC_ENABLE
#   define VSF_TEST_WDT_BASIC_ENABLE           ENABLED
#endif
#ifndef VSF_TEST_WDT_REBOOT_ENABLE
#   define VSF_TEST_WDT_REBOOT_ENABLE          DISABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_wdt_basic_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_wdt_t *wdt;
    )
};

vsf_class(vsf_test_wdt_reboot_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_wdt_t *wdt;
    )
};

typedef struct vsf_test_wdt_suites_t {
    vsf_test_wdt_basic_suite_t basic;
    vsf_test_wdt_reboot_suite_t reboot;
} vsf_test_wdt_suites_t;

/*============================ PROTOTYPES ====================================*/

typedef struct vsf_test_wdt_cfg_t {
    vsf_wdt_t *wdt;
} vsf_test_wdt_cfg_t;

void vsf_test_wdt_init(vsf_test_wdt_suites_t *s, const vsf_test_wdt_cfg_t *cfg);

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
void vsf_test_wdt_basic_add_cases(vsf_test_wdt_basic_suite_t *suite);
void vsf_test_wdt_basic_run(void *arg);
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
void vsf_test_wdt_reboot_add_cases(vsf_test_wdt_reboot_suite_t *suite);
void vsf_test_wdt_reboot_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_WDT_H__ */
/* EOF */
