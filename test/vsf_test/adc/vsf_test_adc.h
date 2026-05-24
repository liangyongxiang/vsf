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

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ADC_ONESHOT_ENABLE
#   define VSF_TEST_ADC_ONESHOT_ENABLE         ENABLED
#endif
#ifndef VSF_TEST_ADC_TEMPERATURE_ENABLE
#   define VSF_TEST_ADC_TEMPERATURE_ENABLE     DISABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_adc_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_adc_t *adc;
    )
};

vsf_class(vsf_test_adc_oneshot_suite_t) {
    public_member(
        implement(vsf_test_adc_suite_base_t)
    )
};

vsf_class(vsf_test_adc_temperature_suite_t) {
    public_member(
        implement(vsf_test_adc_suite_base_t)
    )
};

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
typedef struct vsf_test_adc_oneshot_case_t {
    uint8_t  idx;
    uint8_t  channel;
    uint16_t expected_min;
    uint16_t expected_max;
    vsf_test_adc_oneshot_suite_t *suite;
} vsf_test_adc_oneshot_case_t;
#endif

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
typedef struct vsf_test_adc_temperature_case_t {
    uint8_t  idx;
    uint8_t  channel_count;   /* total ADC channels (e.g. 5 for 4 ext + 1 temp) */
    uint8_t  sensor_channel;  /* internal temperature sensor channel index */
    uint16_t temp_raw_min;    /* minimum plausible raw sample at room temp */
    uint16_t temp_raw_max;    /* maximum plausible raw sample at room temp */
    vsf_test_adc_temperature_suite_t *suite;
} vsf_test_adc_temperature_case_t;
#endif

/*============================ STATIC INIT MACROS ============================*/

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
#define VSF_TEST_ADC_ONESHOT_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_adc_oneshot_suite_t suite_var; \
    static vsf_test_adc_oneshot_case_t __##suite_var##_data[] = { \
        VSF_TEST_ADC_ONESHOT_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_ADC_ONESHOT_CASES(__##suite_var##_data, vsf_test_adc_oneshot_run, false) \
    }; \
    static vsf_test_adc_oneshot_suite_t suite_var = { \
        .name       = name_str, \
        .purpose    = "adc_oneshot", \
        .hw_req     = "none", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
#define VSF_TEST_ADC_TEMPERATURE_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_adc_temperature_suite_t suite_var; \
    static vsf_test_adc_temperature_case_t __##suite_var##_data[] = { \
        VSF_TEST_ADC_TEMPERATURE_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_ADC_TEMPERATURE_CASES(__##suite_var##_data, vsf_test_adc_temperature_run, false) \
    }; \
    static vsf_test_adc_temperature_suite_t suite_var = { \
        .name       = name_str, \
        .purpose    = "adc_temperature", \
        .hw_req     = "none", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
void vsf_test_adc_oneshot_run(void *arg);
#endif

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
void vsf_test_adc_temperature_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_ADC_H__ */
/* EOF */
