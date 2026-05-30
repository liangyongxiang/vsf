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


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_pinmux_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_gpio_pinmux_params_t *p = tc->arg;
    void **handles = (void **)fixture;
    vsf_gpio_t *gpio = (vsf_gpio_t *)handles[0];
    vsf_usart_t *usart = (vsf_usart_t *)handles[1];
    vsf_gpio_pin_mask_t tx_mask = (vsf_gpio_pin_mask_t)1u << p->tx_pin;
    vsf_gpio_pin_mask_t rx_mask = (vsf_gpio_pin_mask_t)1u << p->rx_pin;

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
        .mode = VSF_GPIO_AF,
        .alternate_function = vsf_board_get_uart_funcsel(),
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Step 3: bring up UART and send a small payload. We do not assert
     * loopback receive here because the host script handles the
     * post-condition (UART line bytes captured on LA / serial). */
    err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE,
        .baudrate = p->baudrate,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    const char *payload = "PINMUX\r\n";
    while (*payload) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)payload, 1);
        payload++;
    }
    vsf_test_busy_wait_ms(50);

    /* Tear down so subsequent suites get UART1 in a clean state. Without
     * this, later RX-on-UART1 scenarios (rx_baud, rx_data, ...) fail
     * because the peripheral is left enabled in TX-only mode. */
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);
}

#endif /* VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED */

/* EOF */
