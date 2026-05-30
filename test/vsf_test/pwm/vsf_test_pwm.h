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

#ifndef __VSF_TEST_PWM_H__
#define __VSF_TEST_PWM_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_BASIC_ENABLE
#   define VSF_TEST_PWM_BASIC_ENABLE           ENABLED
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_ENABLE
#   define VSF_TEST_PWM_DUAL_CHANNEL_ENABLE    ENABLED
#endif

#ifndef VSF_TEST_PWM_IRQ_ENABLE
#   define VSF_TEST_PWM_IRQ_ENABLE             ENABLED
#endif

/*============================ TYPES =========================================*/




#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
vsf_class(vsf_test_pwm_basic_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  slice;
        uint8_t  channel;
        uint8_t  gpio;
        uint32_t freq_hz;
        uint32_t period;
        uint32_t pulse;
        uint32_t run_ms;
    )
};
#endif

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
vsf_class(vsf_test_pwm_dual_channel_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  slice;
        uint8_t  channel_a;
        uint8_t  channel_b;
        uint8_t  gpio_a;
        uint8_t  gpio_b;
        uint32_t freq_hz;
        uint32_t period;
        uint32_t pulse_a;
        uint32_t pulse_b;
        uint32_t run_ms;
    )
};
#endif

#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED

vsf_class(vsf_test_pwm_irq_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  slice;
        uint8_t  channel;
        uint32_t freq_hz;
        uint32_t period;
        uint32_t pulse;
        uint32_t test_ms;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
void vsf_test_pwm_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
void vsf_test_pwm_dual_channel_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
void vsf_test_pwm_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#ifdef __cplusplus
}
#endif


/*============================ SUITE TABLE ==================================*/

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
#   define __vsf_test_pwm_basic_suite { .name = "pwm_basic", .cases = __pwm_basic_cases, .case_count = dimof(__pwm_basic_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_PWM },
#else
#   define __vsf_test_pwm_basic_suite
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
#   define __vsf_test_pwm_dual_channel_suite { .name = "pwm_dual_channel", .cases = __pwm_dual_channel_cases, .case_count = dimof(__pwm_dual_channel_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_PWM },
#else
#   define __vsf_test_pwm_dual_channel_suite
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
#   define __vsf_test_pwm_irq_suite { .name = "pwm_irq", .cases = __pwm_irq_cases, .case_count = dimof(__pwm_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_PWM },
#else
#   define __vsf_test_pwm_irq_suite
#endif

#define VSF_TEST_PWM_SUITES \
    __vsf_test_pwm_basic_suite \
    __vsf_test_pwm_dual_channel_suite \
    __vsf_test_pwm_irq_suite

#endif /* __VSF_TEST_PWM_H__ */
/* EOF */
