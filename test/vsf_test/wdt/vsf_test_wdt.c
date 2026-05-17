#include "vsf_test_wdt.h"
#include "scenario/vsf_test_wdt_basic.h"

void vsf_test_wdt_register_all(vsf_test_wdt_scenes_t *s)
{
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_wdt_basic_run,
        "wdt_basic", (void *)s);
#endif
}
