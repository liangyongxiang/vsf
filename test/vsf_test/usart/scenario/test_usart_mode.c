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
#include "test_usart_mode.h"
#include "test_params_generated.h"

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef MODE_PAYLOAD
#   define MODE_PAYLOAD            "Hello VSF\r\n"
#endif
#ifndef MARKER_DELAY_MS
#   define MARKER_DELAY_MS         200
#endif
#ifndef MODE_PAYLOAD_DRAIN_MS
#   define MODE_PAYLOAD_DRAIN_MS   500
#endif
#ifndef MODE_COMMON_BAUDRATE
#   define MODE_COMMON_BAUDRATE    115200
#endif

/*============================ IMPLEMENTATION ================================*/

static void __busy_wait_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 22000; i++);
}

static void __usart_send_str(vsf_usart_t *usart, const char *str)
{
    while (*str) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)str, 1);
        str++;
    }
}

/*============================ TEST CASE =====================================*/

void vsf_test_usart_mode_scenario(void *arg)
{
    const vsf_test_usart_mode_case_t *c = (const vsf_test_usart_mode_case_t *)arg;

    vsf_trace_info("MODE:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    __busy_wait_ms(MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(test_usart_instance, &(vsf_usart_cfg_t){
        .mode     = c->mode,
        .baudrate = MODE_COMMON_BAUDRATE,
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(test_usart_instance));
        __usart_send_str(test_usart_instance, MODE_PAYLOAD);
        __busy_wait_ms(MODE_PAYLOAD_DRAIN_MS);
        while (fsm_rt_cpl != vsf_usart_disable(test_usart_instance));
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}

#endif /* VSF_TEST_USART_TX_MODE_ENABLE == ENABLED */

/* EOF */
