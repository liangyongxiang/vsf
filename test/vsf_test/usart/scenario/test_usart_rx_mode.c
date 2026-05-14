/*****************************************************************************
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

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"
#include "../test_usart.h"
#include "test_usart_rx_mode.h"
#include "test_params_generated.h"

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef RX_MODE_PAYLOAD
#   define RX_MODE_PAYLOAD          "0123456789\r\n"
#endif
#ifndef MARKER_DELAY_MS
#   define MARKER_DELAY_MS          200
#endif
#ifndef RX_MODE_PAYLOAD_DRAIN_MS
#   define RX_MODE_PAYLOAD_DRAIN_MS 500
#endif
#ifndef RX_MODE_COMMON_BAUDRATE
#   define RX_MODE_COMMON_BAUDRATE  115200
#endif

/*============================ IMPLEMENTATION ================================*/

static void __busy_wait_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 22000; i++);
}

/*============================ TEST CASE =====================================*/

void vsf_test_usart_rx_mode_scenario(void *arg)
{
    const vsf_test_usart_rx_mode_case_t *c = (const vsf_test_usart_rx_mode_case_t *)arg;

    vsf_trace_info("RX:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    __busy_wait_ms(MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(test_usart_instance, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = RX_MODE_COMMON_BAUDRATE,
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(test_usart_instance));

        vsf_trace_info("RX:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint8_t rx_buf[32];
        uint16_t rx_len = 0;
        const char *expected = RX_MODE_PAYLOAD;
        uint16_t expected_len = strlen(expected);

        uint32_t timeout_ticks = vsf_systimer_get_ms() + RX_MODE_PAYLOAD_DRAIN_MS * 10;
        while (rx_len < expected_len) {
            uint_fast16_t count = vsf_usart_rxfifo_get_data_count(test_usart_instance);
            while (count-- > 0 && rx_len < sizeof(rx_buf)) {
                vsf_usart_rxfifo_read(test_usart_instance, &rx_buf[rx_len], 1);
                rx_len++;
            }
            if (vsf_systimer_get_ms() > timeout_ticks) {
                break;
            }
        }

        VSF_TEST_ASSERT(rx_len == expected_len);
        VSF_TEST_ASSERT(memcmp(rx_buf, expected, expected_len) == 0);

        while (fsm_rt_cpl != vsf_usart_disable(test_usart_instance));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}

#endif /* VSF_TEST_USART_RX_MODE_ENABLE == ENABLED */

/* EOF */
