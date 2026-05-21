/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
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

#ifndef __VSF_TEST_FLASH_H__
#define __VSF_TEST_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_TEST_FLASH_CASE_MAX_COUNT     8

#ifndef VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE
#   define VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE     DISABLED
#endif

#ifndef VSF_TEST_FLASH_BOUNDARY_ENABLE
#   define VSF_TEST_FLASH_BOUNDARY_ENABLE               DISABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_flash_erase_program_read_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_flash_t *flash;
    )
};

#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
typedef struct vsf_test_flash_erase_program_read_case_t {
    uint8_t  idx;
    uint32_t offset;
    uint32_t size;
    vsf_test_flash_erase_program_read_suite_t *suite;
} vsf_test_flash_erase_program_read_case_t;
#endif

vsf_class(vsf_test_flash_boundary_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_flash_t *flash;
    )
};

#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
typedef struct vsf_test_flash_boundary_case_t {
    uint8_t  idx;
    uint32_t offset;
    uint32_t size;
    vsf_test_flash_boundary_suite_t *suite;
} vsf_test_flash_boundary_case_t;
#endif

typedef struct vsf_test_flash_suites_t {
    vsf_test_flash_erase_program_read_suite_t erase_program_read;
    vsf_test_flash_boundary_suite_t           boundary;
} vsf_test_flash_suites_t;

void vsf_test_flash_register_all(vsf_test_flash_suites_t *s, vsf_flash_t *flash);

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
void vsf_test_flash_erase_program_read_add_cases(vsf_test_flash_erase_program_read_suite_t *suite);
void vsf_test_flash_erase_program_read_run(const vsf_test_flash_erase_program_read_case_t *c);
#endif

#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
void vsf_test_flash_boundary_add_cases(vsf_test_flash_boundary_suite_t *suite);
void vsf_test_flash_boundary_run(const vsf_test_flash_boundary_case_t *c);
#endif

#include "test_params_generated.h"

#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_FLASH_H__ */
/* EOF */
