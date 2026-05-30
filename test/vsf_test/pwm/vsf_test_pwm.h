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

vsf_class(vsf_test_pwm_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_pwm_t *pwm;
    )
};

vsf_class(vsf_test_pwm_basic_suite_t) {
    public_member(
        implement(vsf_test_pwm_suite_base_t)
    )
};

vsf_class(vsf_test_pwm_dual_channel_suite_t) {
    public_member(
        implement(vsf_test_pwm_suite_base_t)
    )
};

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
vsf_class(vsf_test_pwm_basic_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  slice;
        uint8_t  channel;
        uint8_t  gpio;
        uint32_t freq_hz;
        uint32_t period;
        uint32_t pulse;
        uint32_t run_ms;
        vsf_test_pwm_basic_suite_t *suite;
    )
};
#endif

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
vsf_class(vsf_test_pwm_dual_channel_case_t) {
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
        vsf_test_pwm_dual_channel_suite_t *suite;
    )
};
#endif

#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_pwm_irq_suite_t) {
    public_member(
        implement(vsf_test_pwm_suite_base_t)
    )
};

vsf_class(vsf_test_pwm_irq_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  slice;
        uint8_t  channel;
        uint32_t freq_hz;
        uint32_t period;
        uint32_t pulse;
        uint32_t test_ms;
        vsf_test_pwm_irq_suite_t *suite;
    )
};
#endif

/*============================ STATIC TABLE TYPES ============================*/

VSF_TEST_DECLARE_TABLE(vsf_test_pwm_basic_table_t, vsf_test_pwm_basic_suite_t, vsf_test_pwm_basic_case_t, VSF_TEST_PWM_BASIC_CASE_COUNT);
VSF_TEST_DECLARE_TABLE(vsf_test_pwm_dual_channel_table_t, vsf_test_pwm_dual_channel_suite_t, vsf_test_pwm_dual_channel_case_t, VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT);
VSF_TEST_DECLARE_TABLE(vsf_test_pwm_irq_table_t, vsf_test_pwm_irq_suite_t, vsf_test_pwm_irq_case_t, VSF_TEST_PWM_IRQ_CASE_COUNT);

/*============================ STATIC INIT MACROS ============================*/

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
#define VSF_TEST_PWM_BASIC_STATIC(suite_var, name_str) \
    static vsf_test_pwm_basic_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_PWM_BASIC_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_PWM, \
            .pwm        = VSF_BOARD_PWM_INSTANCE, \
        }, \
        .data  = { VSF_TEST_PWM_BASIC_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_PWM_BASIC_CASES(suite_var.data, vsf_test_pwm_basic_run, false) }, \
    }
#endif

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
#define VSF_TEST_PWM_DUAL_CHANNEL_STATIC(suite_var, name_str) \
    static vsf_test_pwm_dual_channel_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_PWM, \
            .pwm        = VSF_BOARD_PWM_INSTANCE, \
        }, \
        .data  = { VSF_TEST_PWM_DUAL_CHANNEL_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_PWM_DUAL_CHANNEL_CASES(suite_var.data, vsf_test_pwm_dual_channel_run, false) }, \
    }
#endif

#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
#define VSF_TEST_PWM_IRQ_STATIC(suite_var, name_str) \
    static vsf_test_pwm_irq_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_PWM_IRQ_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_PWM, \
            .pwm        = VSF_BOARD_PWM_INSTANCE, \
        }, \
        .data  = { VSF_TEST_PWM_IRQ_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_PWM_IRQ_CASES(suite_var.data, vsf_test_pwm_irq_run, false) }, \
    }
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
void vsf_test_pwm_basic_run(void *arg);
#endif

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
void vsf_test_pwm_dual_channel_run(void *arg);
#endif

#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
void vsf_test_pwm_irq_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_PWM_H__ */
/* EOF */
