#ifndef __VSF_TEST_WDT_BASIC_H__
#define __VSF_TEST_WDT_BASIC_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_wdt.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_WDT_BASIC_CASE_COUNT
#   define VSF_TEST_WDT_BASIC_CASE_COUNT       1
#endif

#define VSF_TEST_WDT_BASIC_CASES_INIT                                       \
    { 0 }

/*============================ TYPES =========================================*/

typedef struct vsf_test_wdt_basic_case_t {
    uint8_t idx;
    vsf_test_wdt_basic_suite_t *suite;
} vsf_test_wdt_basic_case_t;

#endif /* __VSF_TEST_WDT_BASIC_H__ */
/* EOF */
