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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __VSF_TEST_ADC_H__
#define __VSF_TEST_ADC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ADC_ONESHOT_ENABLE
#   define VSF_TEST_ADC_ONESHOT_ENABLE         ENABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_adc_oneshot_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_adc_t *adc;
    )
};

typedef struct vsf_test_adc_suites_t {
    vsf_test_adc_oneshot_suite_t oneshot;
} vsf_test_adc_suites_t;

/*============================ PROTOTYPES ====================================*/

void vsf_test_adc_register_all(vsf_test_adc_suites_t *s);

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
void vsf_test_adc_oneshot_add_cases(vsf_test_adc_oneshot_suite_t *suite);
void vsf_test_adc_oneshot_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_ADC_H__ */
/* EOF */
