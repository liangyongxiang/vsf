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

#include "vsf_test_usart_rx_data.h"
/*============================ LOCAL VARIABLES ===============================*/

static uint8_t __rx_data_buf[4096];



#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_DATA_PAYLOAD
#   define VSF_TEST_RX_DATA_PAYLOAD         "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_RX_DATA_PAYLOAD_DRAIN_MS
#   define VSF_TEST_RX_DATA_PAYLOAD_DRAIN_MS 500
#endif
#ifndef VSF_TEST_RX_DATA_DEFAULT_MODE
#   define VSF_TEST_RX_DATA_DEFAULT_MODE     (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_RX_ENABLE)
#endif
#ifndef VSF_TEST_RX_DATA_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_DATA_DEFAULT_BAUDRATE 115200
#endif

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_data_run(vsf_test_case_t *tc)
{
    vsf_test_usart_rx_data_params_t *p = tc->arg;
    vsf_test_suite_t *suite = tc->suite;
    vsf_err_t err = vsf_usart_init((vsf_usart_t *)suite->arg, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_RX_DATA_DEFAULT_MODE,
        .baudrate = VSF_TEST_RX_DATA_DEFAULT_BAUDRATE,
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable((vsf_usart_t *)suite->arg));

        if (p->data_size_bytes > 0) {
            /* Bulk transfer: incrementing-counter pattern */
            uint32_t rx_len = 0;
            uint32_t expected_len = p->data_size_bytes;

            /* Scale timeout: 10 bits/byte @ baudrate, factor of 2 margin */
            uint32_t max_ms = (expected_len * 10 * 2) / (VSF_TEST_RX_DATA_DEFAULT_BAUDRATE / 1000);
            if (max_ms < 5000) { max_ms = 5000; }
            uint32_t elapsed_ms = 0;

            while (rx_len < expected_len && elapsed_ms < max_ms) {
                uint_fast16_t count = vsf_usart_rxfifo_get_data_count((vsf_usart_t *)suite->arg);
                if (count > 0) {
                    uint_fast16_t want = expected_len - rx_len;
                    if (want > count) { want = count; }
                    uint_fast16_t got = vsf_usart_rxfifo_read(
                        (vsf_usart_t *)suite->arg, &__rx_data_buf[rx_len], want);
                    rx_len += got;
                } else {
                    vsf_test_busy_wait_ms(1);
                    elapsed_ms += 1;
                }
            }

            VSF_TEST_ASSERT(rx_len == expected_len);

            for (uint32_t i = 0; i < expected_len; i++) {
                VSF_TEST_ASSERT(__rx_data_buf[i] == (uint8_t)(i & 0xFF));
            }

            vsf_trace_info("USART:RX_DATA:sz=%lu" VSF_TRACE_CFG_LINEEND,
                           (unsigned long)expected_len);
        } else {
            /* Original string-based behavior (case idx 0) */
            uint8_t rx_buf[32];
            uint16_t rx_len = 0;
            const char *expected = VSF_TEST_RX_DATA_PAYLOAD;
            uint16_t expected_len = strlen(expected);

            uint32_t elapsed_ms = 0;
            const uint32_t max_ms = VSF_TEST_RX_DATA_PAYLOAD_DRAIN_MS * 10;
            while (rx_len < expected_len && elapsed_ms < max_ms) {
                uint_fast16_t count = vsf_usart_rxfifo_get_data_count((vsf_usart_t *)suite->arg);
                while (count-- > 0 && rx_len < sizeof(rx_buf)) {
                    vsf_usart_rxfifo_read((vsf_usart_t *)suite->arg, &rx_buf[rx_len], 1);
                    rx_len++;
                }
                vsf_test_busy_wait_ms(10);
                elapsed_ms += 10;
            }

            VSF_TEST_ASSERT(rx_len == expected_len);
            VSF_TEST_ASSERT(memcmp(rx_buf, expected, expected_len) == 0);
        }

        while (fsm_rt_cpl != vsf_usart_disable((vsf_usart_t *)suite->arg));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini((vsf_usart_t *)suite->arg);
}

#endif /* VSF_TEST_USART_RX_DATA_ENABLE == ENABLED */

/* EOF */
