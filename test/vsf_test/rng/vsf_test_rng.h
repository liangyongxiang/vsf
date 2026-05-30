/******************************************************************************
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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __VSF_TEST_RNG_H__
#define __VSF_TEST_RNG_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RNG_BASIC_ENABLE
#   define VSF_TEST_RNG_BASIC_ENABLE           ENABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_rng_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_rng_t *rng;
    )
};

vsf_class(vsf_test_rng_basic_suite_t) {
    public_member(
        implement(vsf_test_rng_suite_base_t)
    )
};

#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
vsf_class(vsf_test_rng_basic_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  word_count;
        vsf_test_rng_basic_suite_t *suite;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
void vsf_test_rng_basic_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_RNG_H__ */
/* EOF */
