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

#include "vsf_test_gpio_pinmux.h"
#include "vsf_board.h"

#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED


static vsf_test_gpio_pinmux_case_t __gpio_pinmux_cases[] = {
    VSF_TEST_GPIO_PINMUX_CASES_INIT
};

void vsf_test_gpio_pinmux_add_cases(vsf_test_gpio_pinmux_suite_t *suite)
{
    suite->name    = "gpio_pinmux";
    suite->purpose = "pinmux";
    suite->hw_req  = "uart1";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_GPIO_PINMUX_CASE_COUNT; i++) {
        __gpio_pinmux_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_gpio_pinmux_run,
            (void *)&__gpio_pinmux_cases[i]);
    }
}

void vsf_test_gpio_pinmux_run(const vsf_test_gpio_pinmux_case_t *c)
{
    vsf_gpio_t *gpio = c->suite->gpio;
    vsf_gpio_pin_mask_t tx_mask = (vsf_gpio_pin_mask_t)1u << c->tx_pin;
    vsf_gpio_pin_mask_t rx_mask = (vsf_gpio_pin_mask_t)1u << c->rx_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    /* Step 1: drive the pins as plain GPIO output to prove they are
     * controllable before we hand them to the UART peripheral. */
    vsf_gpio_port_config_pins(gpio, tx_mask | rx_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_set(gpio, tx_mask | rx_mask);
    vsf_test_busy_wait_ms(1);
    vsf_gpio_clear(gpio, tx_mask | rx_mask);
    vsf_test_busy_wait_ms(1);

    /* Step 2: configure to UART AF via the alternate_function field. */
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, tx_mask | rx_mask, &(vsf_gpio_cfg_t){
        .mode = (5 << 0),       /* VSF_GPIO_AF slot per template */
        .alternate_function = vsf_board_get_uart_funcsel(),
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Step 3: bring up UART and send a small payload. We do not assert
     * loopback receive here because the host script handles the
     * post-condition (UART line bytes captured on LA / serial). */
    err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE,
        .baudrate = c->baudrate,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));

    const char *payload = "PINMUX\r\n";
    while (*payload) {
        while (!vsf_usart_txfifo_get_free_count(c->suite->usart));
        vsf_usart_txfifo_write(c->suite->usart, (uint8_t *)payload, 1);
        payload++;
    }
    vsf_test_busy_wait_ms(50);

    /* Tear down so subsequent suites get UART1 in a clean state. Without
     * this, later RX-on-UART1 scenarios (rx_baud, rx_data, ...) fail
     * because the peripheral is left enabled in TX-only mode. */
    while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    vsf_usart_fini(c->suite->usart);
}

#endif /* VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED */

/* EOF */
