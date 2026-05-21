/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt.h"
#include "scenario/vsf_test_wdt_basic.h"
#include "scenario/vsf_test_wdt_reboot.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware scenarios: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
void vsf_test_wdt_register_all(vsf_test_wdt_suites_t *s)
{
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    vsf_test_wdt_basic_add_cases(&s->basic);
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    vsf_test_wdt_reboot_add_cases(&s->reboot);
#endif
}

/* EOF */
