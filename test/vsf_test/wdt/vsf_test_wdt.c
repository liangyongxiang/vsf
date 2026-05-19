/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt.h"
#include "scenario/vsf_test_wdt_basic.h"

/*============================ IMPLEMENTATION ================================*/

#define REG_IF(gate, s, field, add_fn)            \
    do {                                          \
        vsf_test_shell_register_scene(vsf_test_get_shell(), gate); \
        add_fn(&(s)->field);                      \
    } while (0)

void vsf_test_wdt_register_all(vsf_test_wdt_scenes_t *s)
{
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    REG_IF("wdt_basic", s, basic, vsf_test_wdt_basic_add_cases);
#endif
}

/* EOF */
