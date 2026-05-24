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

#include "vsf_test_usart_rx_mode.h"

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_MODE_PAYLOAD
#   define VSF_TEST_RX_MODE_PAYLOAD          "0123456789\r\n"
#endif
#ifndef VSF_TEST_RX_MODE_PAYLOAD_DRAIN_MS
#   define VSF_TEST_RX_MODE_PAYLOAD_DRAIN_MS 500
#endif
#ifndef VSF_TEST_RX_MODE_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_MODE_DEFAULT_BAUDRATE  115200
#endif

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

VSF_TEST_SUITE_REGISTER(vsf_test_usart_rx_mode_add_cases,
    vsf_test_usart_rx_mode_suite_t,
    vsf_test_usart_rx_mode_case_t,
    vsf_test_usart_rx_mode_run,
    VSF_TEST_USART_RX_MODE_CASES_INIT,
    "usart_rx_mode", "rx-mode", "uart1+la",
    true)

void vsf_test_usart_rx_mode_run(const vsf_test_usart_rx_mode_case_t *c)
{
    vsf_err_t err = vsf_usart_init(c->suite->usart, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = VSF_TEST_RX_MODE_DEFAULT_BAUDRATE,
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->suite->usart));

        uint8_t rx_buf[32];
        uint16_t rx_len = 0;
        const char *expected = VSF_TEST_RX_MODE_PAYLOAD;
        uint16_t expected_len = strlen(expected);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_MODE_PAYLOAD_DRAIN_MS * 10;
        while (rx_len < expected_len && elapsed_ms < max_ms) {
            uint_fast16_t count = vsf_usart_rxfifo_get_data_count(c->suite->usart);
            while (count-- > 0 && rx_len < sizeof(rx_buf)) {
                vsf_usart_rxfifo_read(c->suite->usart, &rx_buf[rx_len], 1);
                rx_len++;
            }
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        VSF_TEST_ASSERT(rx_len == expected_len);
        VSF_TEST_ASSERT(memcmp(rx_buf, expected, expected_len) == 0);

        while (fsm_rt_cpl != vsf_usart_disable(c->suite->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->suite->usart);
}

#endif /* VSF_TEST_USART_RX_MODE_ENABLE == ENABLED */

/* EOF */
