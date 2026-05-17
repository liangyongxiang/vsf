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

#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif

static vsf_test_gpio_pinmux_case_t __gpio_pinmux_cases[] = {
    VSF_TEST_GPIO_PINMUX_CASES_INIT
};

void vsf_test_gpio_pinmux_add_cases(vsf_test_gpio_pinmux_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_GPIO_PINMUX_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_GPIO_CASE_MAX_COUNT][96];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "gpio_pinmux_%u purpose=pinmux hw_req=uart1 tx=%u rx=%u",
            (unsigned)__gpio_pinmux_cases[i].idx,
            (unsigned)__gpio_pinmux_cases[i].tx_pin,
            (unsigned)__gpio_pinmux_cases[i].rx_pin);
        /* Inject the usart pointer via a local mutable copy of the case */
        static vsf_test_gpio_pinmux_case_t __pool[VSF_TEST_GPIO_CASE_MAX_COUNT];
        __pool[i] = __gpio_pinmux_cases[i];
        __pool[i].scene = scene;
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_gpio_pinmux_run,
            __cfg_str_pool[i], (void *)&__pool[i]);
    }
}

void vsf_test_gpio_pinmux_run(const vsf_test_gpio_pinmux_case_t *c)
{
    vsf_gpio_t *gpio = c->scene->gpio;
    vsf_gpio_pin_mask_t tx_mask = (vsf_gpio_pin_mask_t)1u << c->tx_pin;
    vsf_gpio_pin_mask_t rx_mask = (vsf_gpio_pin_mask_t)1u << c->rx_pin;

    vsf_trace_info("GPIO:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

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
        .alternate_function = c->uart_funcsel,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Step 3: bring up UART and send a small payload. We do not assert
     * loopback receive here because the host script handles the
     * post-condition (UART line bytes captured on LA / serial). */
    err = vsf_usart_init(c->scene->usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE,
        .baudrate = c->baudrate,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable(c->scene->usart));

    const char *payload = "PINMUX\r\n";
    while (*payload) {
        while (!vsf_usart_txfifo_get_free_count(c->scene->usart));
        vsf_usart_txfifo_write(c->scene->usart, (uint8_t *)payload, 1);
        payload++;
    }
    vsf_test_busy_wait_ms(50);
}

#endif /* VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED */

/* EOF */
