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
#ifndef VSF_TEST_ADC_STREAM_ENABLE
#   define VSF_TEST_ADC_STREAM_ENABLE          ENABLED
#endif

/*============================ TYPES =========================================*/




#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
vsf_class(vsf_test_adc_oneshot_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  channel;
        uint16_t expected_min;
        uint16_t expected_max;
    )
};
#endif

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
vsf_class(vsf_test_adc_temperature_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  channel_count;   /* total ADC channels (e.g. 5 for 4 ext + 1 temp) */
        uint8_t  sensor_channel;  /* internal temperature sensor channel index */
        uint16_t temp_raw_min;    /* minimum plausible raw sample at room temp */
        uint16_t temp_raw_max;    /* maximum plausible raw sample at room temp */
    )
};
#endif

#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED

vsf_class(vsf_test_adc_stream_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  channel;
        uint16_t sample_count;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
void vsf_test_adc_oneshot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
void vsf_test_adc_temperature_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
void vsf_test_adc_stream_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_ADC_H__ */
/* EOF */
