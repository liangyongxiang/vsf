/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  You may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

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

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
typedef struct vsf_test_wdt_basic_case_t {
    uint8_t  idx;
    uint16_t timeout_ms;
    uint8_t  feed_count;
    uint16_t feed_interval_ms;
    vsf_test_wdt_basic_suite_t *suite;
} vsf_test_wdt_basic_case_t;
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
typedef struct vsf_test_wdt_reboot_case_t {
    uint8_t  idx;
    uint16_t timeout_ms;
    vsf_test_wdt_reboot_suite_t *suite;
} vsf_test_wdt_reboot_case_t;
#endif

/*============================ STATIC INIT MACROS ============================*/

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
#define VSF_TEST_WDT_BASIC_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_wdt_basic_suite_t suite_var; \
    static vsf_test_wdt_basic_case_t __##suite_var##_data[] = { \
        VSF_TEST_WDT_BASIC_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_WDT_BASIC_CASES(__##suite_var##_data, vsf_test_wdt_basic_run, false) \
    }; \
    static vsf_test_wdt_basic_suite_t suite_var = { \
        .name       = name_str, \
        .purpose    = "wdt_basic", \
        .hw_req     = "none", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
#define VSF_TEST_WDT_REBOOT_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_wdt_reboot_suite_t suite_var; \
    static vsf_test_wdt_reboot_case_t __##suite_var##_data[] = { \
        VSF_TEST_WDT_REBOOT_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_WDT_REBOOT_CASES(__##suite_var##_data, vsf_test_wdt_reboot_run, false) \
    }; \
    static vsf_test_wdt_reboot_suite_t suite_var = { \
        .name       = name_str, \
        .purpose    = "wdt_reboot", \
        .hw_req     = "none", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
void vsf_test_wdt_basic_run(void *arg);
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
void vsf_test_wdt_reboot_run(void *arg);
#endif

#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_WDT_H__ */
/* EOF */
