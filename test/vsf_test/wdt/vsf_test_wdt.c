/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt.h"
#include "suite/vsf_test_wdt_basic.h"
#include "suite/vsf_test_wdt_reboot.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
vsf_test_wdt_suites_t vsf_test_wdt_suites;

void vsf_test_wdt_init(vsf_test_wdt_suites_t *s,
                         const vsf_test_wdt_suite_binding_t bindings[],
                         uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        vsf_test_wdt_suite_base_t *suite = bindings[i].suite;
        vsf_wdt_t                *inst  = bindings[i].instance;
        if (inst == NULL) { continue; }

        suite->wdt  = inst;
        suite->setup  = bindings[i].setup;
        suite->teardown = bindings[i].teardown;
    }
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    vsf_test_wdt_basic_add_cases(&s->basic);
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    vsf_test_wdt_reboot_add_cases(&s->reboot);
#endif

}


/* EOF */
