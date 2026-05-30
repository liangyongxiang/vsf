#ifndef __VSF_TEST_ADC_STREAM_H__
#define __VSF_TEST_ADC_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_adc.h"

#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
typedef struct {
    volatile bool completed;
} vsf_test_adc_stream_var_t;
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ADC_STREAM_CASE_COUNT
#   define VSF_TEST_ADC_STREAM_CASE_COUNT   1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_adc_stream_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_ADC_STREAM_H__ */
/* EOF */
