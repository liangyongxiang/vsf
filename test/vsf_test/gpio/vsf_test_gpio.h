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

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_TEST_GPIO_CASE_MAX_COUNT    16

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

/*============================ TYPES =========================================*/

#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
typedef struct vsf_test_gpio_output_input_case_t {
    uint8_t idx;
    uint8_t out_pin;
    uint8_t in_pin;
    vsf_test_gpio_output_input_scene_t *scene;
} vsf_test_gpio_output_input_case_t;
#endif

#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
typedef struct vsf_test_gpio_toggle_case_t {
    uint8_t idx;
    uint8_t out_pin;
    uint8_t in_pin;
    vsf_test_gpio_toggle_scene_t *scene;
} vsf_test_gpio_toggle_case_t;
#endif

#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
typedef struct vsf_test_gpio_direction_case_t {
    uint8_t idx;
    uint8_t pin;
    vsf_test_gpio_direction_scene_t *scene;
} vsf_test_gpio_direction_case_t;
#endif

#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
typedef struct vsf_test_gpio_atomic_case_t {
    uint8_t idx;
    uint8_t out_pin;        //! pin manipulated atomically
    uint8_t in_pin;         //! pin reading the loopback
    vsf_test_gpio_atomic_scene_t *scene;
} vsf_test_gpio_atomic_case_t;
#endif

#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
typedef struct vsf_test_gpio_pinmux_case_t {
    uint8_t  idx;
    uint8_t  tx_pin;        //! GP pin to be muxed to UART TX
    uint8_t  rx_pin;        //! GP pin to be muxed to UART RX
    uint8_t  uart_funcsel;  //! FUNCSEL value (RP2040 GPIO_FUNC_UART = 2)
    uint32_t baudrate;
    vsf_usart_t *usart;     //! UART instance backing the pins
    vsf_test_gpio_pinmux_scene_t *scene;
} vsf_test_gpio_pinmux_case_t;
#endif

#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
typedef struct vsf_test_gpio_multi_pin_case_t {
    uint8_t idx;
    uint8_t out_pin_a;
    uint8_t out_pin_b;
    uint8_t in_pin_a;
    uint8_t in_pin_b;
    vsf_test_gpio_multi_pin_scene_t *scene;
} vsf_test_gpio_multi_pin_case_t;
#endif

#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
typedef struct vsf_test_gpio_open_drain_case_t {
    uint8_t idx;
    uint8_t out_pin;
    uint8_t in_pin;
    vsf_test_gpio_open_drain_scene_t *scene;
} vsf_test_gpio_open_drain_case_t;
#endif

#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
typedef struct vsf_test_gpio_toggle_freq_case_t {
    uint8_t  idx;
    uint8_t  pin;
    uint32_t toggle_count;
    vsf_test_gpio_toggle_freq_scene_t *scene;
} vsf_test_gpio_toggle_freq_case_t;
#endif

#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
typedef struct vsf_test_gpio_write_throughput_case_t {
    uint8_t  idx;
    uint8_t  pin;
    uint32_t duration_us;
    vsf_test_gpio_write_throughput_scene_t *scene;
} vsf_test_gpio_write_throughput_case_t;
#endif

#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
typedef struct vsf_test_gpio_toggle_stress_case_t {
    uint8_t  idx;
    uint8_t  out_pin;
    uint8_t  in_pin;
    uint32_t stress_count;
    vsf_test_gpio_toggle_stress_scene_t *scene;
} vsf_test_gpio_toggle_stress_case_t;
#endif

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
typedef struct vsf_test_gpio_concurrent_prio_case_t {
    uint8_t  idx;
    uint8_t  out_pin;
    uint8_t  in_pin;
    uint32_t duration_ms;
    uint32_t callback_period_us;
    vsf_test_gpio_concurrent_prio_scene_t *scene;
} vsf_test_gpio_concurrent_prio_case_t;
#endif

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
typedef struct vsf_test_gpio_exti_case_t {
    uint8_t idx;
    uint8_t pin;
    vsf_test_gpio_exti_scene_t *scene;
} vsf_test_gpio_exti_case_t;
#endif

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
typedef struct vsf_test_gpio_irq_latency_case_t {
    uint8_t  idx;
    uint8_t  pin;
    uint32_t max_latency_ns;
    vsf_test_gpio_irq_latency_scene_t *scene;
} vsf_test_gpio_irq_latency_case_t;
#endif

#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
typedef struct vsf_test_gpio_irq_lifecycle_case_t {
    uint8_t idx;
    uint8_t pin;
    vsf_test_gpio_irq_lifecycle_scene_t *scene;
} vsf_test_gpio_irq_lifecycle_case_t;
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
void vsf_test_gpio_output_input_add_cases(vsf_test_gpio_output_input_scene_t *scene);
void vsf_test_gpio_output_input_run(const vsf_test_gpio_output_input_case_t *c);
#endif

#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
void vsf_test_gpio_toggle_add_cases(vsf_test_gpio_toggle_scene_t *scene);
void vsf_test_gpio_toggle_run(const vsf_test_gpio_toggle_case_t *c);
#endif

#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
void vsf_test_gpio_direction_add_cases(vsf_test_gpio_direction_scene_t *scene);
void vsf_test_gpio_direction_run(const vsf_test_gpio_direction_case_t *c);
#endif

#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
void vsf_test_gpio_atomic_add_cases(vsf_test_gpio_atomic_scene_t *scene);
void vsf_test_gpio_atomic_run(const vsf_test_gpio_atomic_case_t *c);
#endif

#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
void vsf_test_gpio_pinmux_add_cases(vsf_gpio_t *gpio_instance, vsf_usart_t *usart);
void vsf_test_gpio_pinmux_run(const vsf_test_gpio_pinmux_case_t *c);
#endif

#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
void vsf_test_gpio_multi_pin_add_cases(vsf_test_gpio_multi_pin_scene_t *scene);
void vsf_test_gpio_multi_pin_run(const vsf_test_gpio_multi_pin_case_t *c);
#endif

#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
void vsf_test_gpio_open_drain_add_cases(vsf_test_gpio_open_drain_scene_t *scene);
void vsf_test_gpio_open_drain_run(const vsf_test_gpio_open_drain_case_t *c);
#endif

#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
void vsf_test_gpio_toggle_freq_add_cases(vsf_test_gpio_toggle_freq_scene_t *scene);
void vsf_test_gpio_toggle_freq_run(const vsf_test_gpio_toggle_freq_case_t *c);
#endif

#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
void vsf_test_gpio_write_throughput_add_cases(vsf_test_gpio_write_throughput_scene_t *scene);
void vsf_test_gpio_write_throughput_run(const vsf_test_gpio_write_throughput_case_t *c);
#endif

#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
void vsf_test_gpio_toggle_stress_add_cases(vsf_test_gpio_toggle_stress_scene_t *scene);
void vsf_test_gpio_toggle_stress_run(const vsf_test_gpio_toggle_stress_case_t *c);
#endif

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
void vsf_test_gpio_concurrent_prio_add_cases(vsf_test_gpio_concurrent_prio_scene_t *scene);
void vsf_test_gpio_concurrent_prio_run(const vsf_test_gpio_concurrent_prio_case_t *c);
#endif

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
void vsf_test_gpio_exti_add_cases(vsf_test_gpio_exti_scene_t *scene);
void vsf_test_gpio_exti_run(const vsf_test_gpio_exti_case_t *c);
#endif

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
void vsf_test_gpio_irq_latency_add_cases(vsf_test_gpio_irq_latency_scene_t *scene);
void vsf_test_gpio_irq_latency_run(const vsf_test_gpio_irq_latency_case_t *c);
#endif

#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
void vsf_test_gpio_irq_lifecycle_add_cases(vsf_test_gpio_irq_lifecycle_scene_t *scene);
void vsf_test_gpio_irq_lifecycle_run(const vsf_test_gpio_irq_lifecycle_case_t *c);
#endif

#include "test_params_generated.h"

#ifdef __cplusplus
}
#endif

#endif  /* __VSF_TEST_GPIO_H__ */
/* EOF */
