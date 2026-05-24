#ifndef __TEST_ADC_TEMPERATURE_H__
#define __TEST_ADC_TEMPERATURE_H__

#include "../vsf_test_adc.h"

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED

#ifndef VSF_TEST_ADC_TEMPERATURE_CASE_COUNT
#   define VSF_TEST_ADC_TEMPERATURE_CASE_COUNT     1
#endif

#define VSF_TEST_ADC_TEMPERATURE_CASES_INIT                                     \
    { 0 }

#ifdef __cplusplus
extern "C" {
#endif

void vsf_test_adc_temperature_add_cases(vsf_test_adc_temperature_suite_t *suite);
void vsf_test_adc_temperature_run(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED */

#endif /* __TEST_ADC_TEMPERATURE_H__ */
/* EOF */
