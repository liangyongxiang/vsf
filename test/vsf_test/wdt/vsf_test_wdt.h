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




#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
vsf_class(vsf_test_wdt_basic_params_t) {
    public_member(
        uint8_t  idx;
        uint16_t timeout_ms;
        uint8_t  feed_count;
        uint16_t feed_interval_ms;
    )
};
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
vsf_class(vsf_test_wdt_reboot_params_t) {
    public_member(
        uint8_t  idx;
        uint16_t timeout_ms;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
void vsf_test_wdt_basic_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
void vsf_test_wdt_reboot_run(vsf_test_case_t *tc);
#endif

#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_WDT_H__ */
/* EOF */
