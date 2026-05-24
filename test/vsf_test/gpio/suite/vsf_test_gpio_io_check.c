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

/*============================ INCLUDES ======================================*/

#include "vsf_test_gpio_io_check.h"

#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/* Number of rounds to repeat the per-pin bit-bang sequence.
 * Multiple rounds ensure at least one full capture even if the LA
 * started mid-round. */
#ifndef VSF_TEST_GPIO_IO_CHECK_ROUNDS
#   define VSF_TEST_GPIO_IO_CHECK_ROUNDS    3
#endif

/* Idle time between rounds (microseconds). */
#ifndef VSF_TEST_GPIO_IO_CHECK_ROUND_GAP_US
#   define VSF_TEST_GPIO_IO_CHECK_ROUND_GAP_US  1000
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_gpio_io_check_case_t __gpio_io_check_cases[] = {
    VSF_TEST_GPIO_IO_CHECK_CASES_INIT
};

/*============================ LOCAL FUNCTIONS ===============================*/

/* Bit-bang one UART 8N1 frame at the given baudrate.
 * byte: data to send (unique per pin: 0x50 + pin).
 * bit_period_us: baudrate-derived bit period in microseconds.
 *
 * Frame format: 1 start bit (low), 8 data bits LSB-first, 1 stop bit (high).
 */
static void __gpio_bitbang_uart_byte(vsf_gpio_t *gpio,
                                     vsf_gpio_pin_mask_t pin_mask,
                                     uint8_t byte,
                                     uint32_t bit_period_us)
{
    /* Start bit (low). */
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_us(bit_period_us);

    /* 8 data bits, LSB first. */
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (byte & (1u << bit)) {
            vsf_gpio_set(gpio, pin_mask);
        } else {
            vsf_gpio_clear(gpio, pin_mask);
        }
        vsf_test_busy_wait_us(bit_period_us);
    }

    /* Stop bit (high). */
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_us(bit_period_us);
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_io_check_add_cases(vsf_test_gpio_io_check_suite_t *suite)
{
    suite->name    = "gpio_io_check";
    suite->purpose = "io-check";
    suite->hw_req  = "la";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_IO_CHECK_CASE_COUNT; i++) {
        __gpio_io_check_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_io_check_run,
            (void *)&__gpio_io_check_cases[i]);
    }
}

void vsf_test_gpio_io_check_run(const vsf_test_gpio_io_check_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << c->pin;

    /* Bit period = 1e6 / baudrate (microseconds).
     * 115200 baud → ~8.68 µs.  Round-to-nearest to keep the actual baudrate
     * within the DSView UART decoder tolerance (≈ ±3 %).
     *
     * Pins declared in gpio.yml: GP8 (uart1_tx) and GP9 (uart1_rx).
     * Both are safe as GPIO output when UART1 is not in use. */
    uint32_t bit_period_us = (1000000u + c->baudrate / 2) / c->baudrate;
    uint8_t byte = 0x50 + c->pin;

    /* Configure pin as push-pull output, idle high (UART idle state). */
    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_set(gpio, pin_mask);

    /* Bit-bang the unique byte for this pin across multiple rounds. */
    for (uint8_t round = 0; round < VSF_TEST_GPIO_IO_CHECK_ROUNDS; round++) {
        __gpio_bitbang_uart_byte(gpio, pin_mask, byte, bit_period_us);
        vsf_test_busy_wait_us(VSF_TEST_GPIO_IO_CHECK_ROUND_GAP_US);
    }

    vsf_trace_info("GPIO:IO_CHECK:pin=%u byte=0x%02x rounds=%u" VSF_TRACE_CFG_LINEEND,
                   (unsigned)c->pin, (unsigned)byte,
                   (unsigned)VSF_TEST_GPIO_IO_CHECK_ROUNDS);
}

#endif /* VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED */

/* EOF */
