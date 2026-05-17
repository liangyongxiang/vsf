#ifndef __VSF_TEST_WDT_H__
#define __VSF_TEST_WDT_H__

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

typedef struct vsf_test_wdt_scenes_t {
    struct {
        vsf_wdt_t *wdt;
    } basic;
} vsf_test_wdt_scenes_t;

extern void vsf_test_wdt_register_all(vsf_test_wdt_scenes_t *s);

#endif
