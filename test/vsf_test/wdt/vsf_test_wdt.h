#ifndef __VSF_TEST_WDT_H__
#define __VSF_TEST_WDT_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_WDT_BASIC_ENABLE
#   define VSF_TEST_WDT_BASIC_ENABLE           ENABLED
#endif
#ifndef VSF_TEST_WDT_REBOOT_ENABLE
#   define VSF_TEST_WDT_REBOOT_ENABLE          DISABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_wdt_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_wdt_t *wdt;
    )
};

vsf_class(vsf_test_wdt_basic_suite_t) {
    public_member(
        implement(vsf_test_wdt_suite_base_t)
    )
};

vsf_class(vsf_test_wdt_reboot_suite_t) {
    public_member(
        implement(vsf_test_wdt_suite_base_t)
    )
};

typedef struct vsf_test_wdt_suites_t {
    vsf_test_wdt_basic_suite_t basic;
    vsf_test_wdt_reboot_suite_t reboot;
} vsf_test_wdt_suites_t;


extern vsf_test_wdt_suites_t vsf_test_wdt_suites;
/*============================ PROTOTYPES ====================================*/

typedef struct vsf_test_wdt_suite_binding_t {
    vsf_test_wdt_suite_base_t *suite;
    vsf_wdt_t               *instance;   //!< NULL = skip this suite
    bool (*setup)(vsf_test_suite_t *);
    void (*teardown)(vsf_test_suite_t *);
} vsf_test_wdt_suite_binding_t;

void vsf_test_wdt_init(vsf_test_wdt_suites_t *s,
                         const vsf_test_wdt_suite_binding_t bindings[],
                         uint8_t count);

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
void vsf_test_wdt_basic_run(void *arg);
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
void vsf_test_wdt_reboot_run(void *arg);
#endif

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
void vsf_test_wdt_basic_add_cases(vsf_test_wdt_basic_suite_t *suite);
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
void vsf_test_wdt_reboot_add_cases(vsf_test_wdt_reboot_suite_t *suite);
#endif
#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_WDT_H__ */
/* EOF */
