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
#include "test_usart_baud.h"

/*============================ MACROS ========================================*/

#define TEST_PAYLOAD        "Hello VSF\r\n"
#define MARKER_DELAY_MS     200
#define PAYLOAD_DRAIN_MS    500

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

void vsf_test_usart_baud_scenario(void *arg)
{
    const vsf_test_usart_baud_case_t *c = (const vsf_test_usart_baud_case_t *)arg;

    vsf_trace_info("CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    __busy_wait_ms(MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(test_usart_instance, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY | VSF_USART_TX_ENABLE,
        .baudrate = c->baudrate,
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(test_usart_instance));
        __usart_send_str(test_usart_instance, TEST_PAYLOAD);
        __busy_wait_ms(PAYLOAD_DRAIN_MS);
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}

/* EOF */
