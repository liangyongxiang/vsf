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

typedef struct vsf_test_adc_suites_t {
    vsf_test_adc_oneshot_suite_t     oneshot;
    vsf_test_adc_temperature_suite_t temperature;
} vsf_test_adc_suites_t;


extern vsf_test_adc_suites_t vsf_test_adc_suites;
/*============================ PROTOTYPES ====================================*/

typedef struct vsf_test_adc_suite_binding_t {
    vsf_test_adc_suite_base_t *suite;
    vsf_adc_t               *instance;   //!< NULL = skip this suite
    bool (*setup)(vsf_test_suite_t *);
    void (*teardown)(vsf_test_suite_t *);
} vsf_test_adc_suite_binding_t;

void vsf_test_adc_init(vsf_test_adc_suites_t *s,
                         const vsf_test_adc_suite_binding_t bindings[],
                         uint8_t count);

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
void vsf_test_adc_oneshot_run(void *arg);
#endif

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
void vsf_test_adc_temperature_run(void *arg);
#endif

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
void vsf_test_adc_oneshot_add_cases(vsf_test_adc_oneshot_suite_t *suite);
#endif

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
void vsf_test_adc_temperature_add_cases(vsf_test_adc_temperature_suite_t *suite);
#endif
#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_ADC_H__ */
/* EOF */
