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

#include "vsf_test_usart_rx_baud.h"

#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_BAUD_PAYLOAD
#   define VSF_TEST_RX_BAUD_PAYLOAD          "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS          200
#endif
#ifndef VSF_TEST_RX_BAUD_PAYLOAD_DRAIN_MS
#   define VSF_TEST_RX_BAUD_PAYLOAD_DRAIN_MS 500
#endif
#ifndef VSF_TEST_RX_BAUD_DEFAULT_MODE
#   define VSF_TEST_RX_BAUD_DEFAULT_MODE      (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_RX_ENABLE)
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_usart_rx_baud_case_t __rx_baud_cases[] = {
    VSF_TEST_RX_BAUD_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_baud_add_cases(vsf_test_usart_rx_baud_scene_t *scene)
{
    scene->name    = "usart_rx_baud";
    scene->purpose = "rx-baud";
    scene->hw_req  = "uart1+la";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_RX_BAUD_CASE_COUNT; i++) {
        __rx_baud_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_usart_rx_baud_run,
            (void *)&__rx_baud_cases[i]);
    }
}

void vsf_test_usart_rx_baud_run(const vsf_test_usart_rx_baud_case_t *c)
{
    /* Dispatcher (vsf_test_run_case) emits "RX_BAUD:CASE:%d" / ":DONE"
     * Capture Markers and the settle delay; suite-aware scenarios do
     * not print them. */
    vsf_usart_capability_t cap = vsf_usart_capability(c->scene->usart);
    bool expect_pass = (c->baudrate >= cap.min_baudrate)
                    && (c->baudrate <= cap.max_baudrate)
                    && (c->baudrate != 0);

    vsf_err_t err = vsf_usart_init(c->scene->usart, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_RX_BAUD_DEFAULT_MODE,
        .baudrate = c->baudrate,
    });

    if (expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->scene->usart));

        vsf_trace_info("usart_rx_baud:CASE:%d:READY" VSF_TRACE_CFG_LINEEND, (int)c->idx);

        uint8_t rx_buf[32];
        uint16_t rx_len = 0;
        const char *expected = VSF_TEST_RX_BAUD_PAYLOAD;
        uint16_t expected_len = strlen(expected);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_BAUD_PAYLOAD_DRAIN_MS * 10;
        while (rx_len < expected_len && elapsed_ms < max_ms) {
            uint_fast16_t count = vsf_usart_rxfifo_get_data_count(c->scene->usart);
            while (count-- > 0 && rx_len < sizeof(rx_buf)) {
                vsf_usart_rxfifo_read(c->scene->usart, &rx_buf[rx_len], 1);
                rx_len++;
            }
            vsf_test_busy_wait_ms(10);
            elapsed_ms += 10;
        }

        VSF_TEST_ASSERT(rx_len == expected_len);
        VSF_TEST_ASSERT(memcmp(rx_buf, expected, expected_len) == 0);

        while (fsm_rt_cpl != vsf_usart_disable(c->scene->usart));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    vsf_usart_fini(c->scene->usart);
}

#endif /* VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED */

/* EOF */
