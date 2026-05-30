#ifndef __VSF_TEST_ADC_ONESHOT_H__
#define __VSF_TEST_ADC_ONESHOT_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_adc.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ADC_ONESHOT_CASE_COUNT
#   define VSF_TEST_ADC_ONESHOT_CASE_COUNT     1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_adc_oneshot_run(vsf_test_case_t *tc);

#endif /* __VSF_TEST_ADC_ONESHOT_H__ */
/* EOF */
