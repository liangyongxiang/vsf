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

/*============================ TYPES =========================================*/

vsf_class(vsf_test_wdt_basic_scene_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_wdt_t *wdt;
    )
};

typedef struct vsf_test_wdt_scenes_t {
    vsf_test_wdt_basic_scene_t basic;
} vsf_test_wdt_scenes_t;

/*============================ PROTOTYPES ====================================*/

void vsf_test_wdt_register_all(vsf_test_wdt_scenes_t *s);

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
void vsf_test_wdt_basic_add_cases(vsf_test_wdt_basic_scene_t *scene);
void vsf_test_wdt_basic_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_WDT_H__ */
/* EOF */
