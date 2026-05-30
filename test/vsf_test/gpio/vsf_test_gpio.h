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

#ifndef __VSF_TEST_GPIO_H__
#define __VSF_TEST_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#if     defined(__VSF_TEST_GPIO_CLASS_IMPLEMENT)
#   undef __VSF_TEST_GPIO_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#define VSF_TEST_GPIO_CASE_MAX_COUNT    16

/* Phase-3 API check (usart-gpio-coverage-gaps PRD): capability() must
 * report a non-empty pin set. Catches drivers that link but never wired
 * up the capability struct. */
#define VSF_TEST_GPIO_ASSERT_CAPABILITY(__gpio)                          \
    do {                                                                 \
        vsf_gpio_capability_t __cap = vsf_gpio_capability(__gpio);       \
        VSF_TEST_ASSERT(__cap.pin_count > 0);                            \
        VSF_TEST_ASSERT(__cap.pin_mask  != 0);                           \
    } while (0)

#ifndef VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE
#   define VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE     DISABLED
#endif
#ifndef VSF_TEST_GPIO_TOGGLE_ENABLE
#   define VSF_TEST_GPIO_TOGGLE_ENABLE           DISABLED
#endif
#ifndef VSF_TEST_GPIO_DIRECTION_ENABLE
#   define VSF_TEST_GPIO_DIRECTION_ENABLE        DISABLED
#endif
#ifndef VSF_TEST_GPIO_ATOMIC_ENABLE
#   define VSF_TEST_GPIO_ATOMIC_ENABLE           DISABLED
#endif
#ifndef VSF_TEST_GPIO_PINMUX_ENABLE
#   define VSF_TEST_GPIO_PINMUX_ENABLE           DISABLED
#endif
#ifndef VSF_TEST_GPIO_MULTI_PIN_ENABLE
#   define VSF_TEST_GPIO_MULTI_PIN_ENABLE        DISABLED
#endif
#ifndef VSF_TEST_GPIO_OPEN_DRAIN_ENABLE
#   define VSF_TEST_GPIO_OPEN_DRAIN_ENABLE       DISABLED
#endif
#ifndef VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE
#   define VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE      DISABLED
#endif
#ifndef VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE
#   define VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE DISABLED
#endif
#ifndef VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE
#   define VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE    DISABLED
#endif
#ifndef VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE
#   define VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE  DISABLED
#endif
#ifndef VSF_TEST_GPIO_EXTI_ENABLE
#   define VSF_TEST_GPIO_EXTI_ENABLE             DISABLED
#endif
#ifndef VSF_TEST_GPIO_IRQ_LATENCY_ENABLE
#   define VSF_TEST_GPIO_IRQ_LATENCY_ENABLE      DISABLED
#endif
#ifndef VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE
#   define VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE    DISABLED
#endif
#ifndef VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE
#   define VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE  DISABLED
#endif
#ifndef VSF_TEST_GPIO_ANALOG_MODE_ENABLE
#   define VSF_TEST_GPIO_ANALOG_MODE_ENABLE      DISABLED
#endif
#ifndef VSF_TEST_GPIO_IO_CHECK_ENABLE
#   define VSF_TEST_GPIO_IO_CHECK_ENABLE          DISABLED
#endif

/*============================ TYPES =========================================*/

// Per-suite context (populated by __vsf_test in main.c)


















#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
vsf_class(vsf_test_gpio_output_input_params_t) {
    public_member(
        uint8_t idx;
        uint8_t out_pin;
        uint8_t in_pin;
    )
};
#endif

#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
vsf_class(vsf_test_gpio_toggle_params_t) {
    public_member(
        uint8_t idx;
        uint8_t out_pin;
        uint8_t in_pin;
    )
};
#endif

#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
vsf_class(vsf_test_gpio_direction_params_t) {
    public_member(
        uint8_t idx;
        uint8_t pin;
    )
};
#endif

#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
vsf_class(vsf_test_gpio_atomic_params_t) {
    public_member(
        uint8_t idx;
        uint8_t out_pin;        //! pin manipulated atomically
        uint8_t in_pin;         //! pin reading the loopback
    )
};
#endif

#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
vsf_class(vsf_test_gpio_pinmux_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  tx_pin;        //! GP pin to be muxed to UART TX
        uint8_t  rx_pin;        //! GP pin to be muxed to UART RX
        uint32_t baudrate;
        vsf_usart_t *usart;     //! UART instance backing the pins
    )
};
#endif

#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
vsf_class(vsf_test_gpio_multi_pin_params_t) {
    public_member(
        uint8_t idx;
        uint8_t out_pin_a;
        uint8_t out_pin_b;
        uint8_t in_pin_a;
        uint8_t in_pin_b;
    )
};
#endif

#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
vsf_class(vsf_test_gpio_open_drain_params_t) {
    public_member(
        uint8_t idx;
        uint8_t out_pin;
        uint8_t in_pin;
    )
};
#endif

#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
vsf_class(vsf_test_gpio_toggle_freq_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  pin;
        uint32_t toggle_count;
    )
};
#endif

#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
vsf_class(vsf_test_gpio_write_throughput_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  pin;
        uint32_t duration_us;
    )
};
#endif

#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
vsf_class(vsf_test_gpio_toggle_stress_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  out_pin;
        uint8_t  in_pin;
        uint32_t stress_count;
    )
};
#endif

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
vsf_class(vsf_test_gpio_concurrent_prio_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  out_pin;
        uint8_t  in_pin;
        uint32_t duration_ms;
        uint32_t callback_period_us;
    )
};
#endif

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
vsf_class(vsf_test_gpio_exti_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  out_pin;
        uint8_t  in_pin;
        uint32_t trigger_mode;       //! one of VSF_GPIO_EXTI_MODE_* (e.g. _FALLING)
    )
};
#endif

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
vsf_class(vsf_test_gpio_irq_latency_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  pin;
        uint32_t max_latency_ns;
    )
};
#endif

#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
vsf_class(vsf_test_gpio_irq_lifecycle_params_t) {
    public_member(
        uint8_t idx;
        uint8_t pin;
    )
};
#endif

#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
vsf_class(vsf_test_gpio_systimer_health_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  pin;
        uint32_t interval_ms;
        uint32_t toggle_count;
    )
};
#endif

#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
vsf_class(vsf_test_gpio_analog_mode_params_t) {
    public_member(
        uint8_t idx;
        uint8_t pin;
    )
};
#endif

#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
vsf_class(vsf_test_gpio_io_check_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  pin;
        uint32_t baudrate;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
void vsf_test_gpio_output_input_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
void vsf_test_gpio_toggle_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
void vsf_test_gpio_direction_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
void vsf_test_gpio_atomic_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
void vsf_test_gpio_pinmux_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
void vsf_test_gpio_multi_pin_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
void vsf_test_gpio_open_drain_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
void vsf_test_gpio_toggle_freq_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
void vsf_test_gpio_write_throughput_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
void vsf_test_gpio_toggle_stress_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
void vsf_test_gpio_concurrent_prio_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
void vsf_test_gpio_exti_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
void vsf_test_gpio_irq_latency_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
void vsf_test_gpio_irq_lifecycle_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
void vsf_test_gpio_systimer_health_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
void vsf_test_gpio_analog_mode_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
void vsf_test_gpio_io_check_run(vsf_test_case_t *tc);
#endif

// Framework types — included LAST so this header can be pulled into
// vsf_test.h without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif  /* __VSF_TEST_GPIO_H__ */
/* EOF */
