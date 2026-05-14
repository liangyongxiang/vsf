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

/*============================ GLOBAL VARIABLES ==============================*/

const uint32_t *test_usart_baudrates = NULL;

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

static void __run_baud_test(vsf_usart_t *usart, uint8_t case_idx, uint32_t baud)
{
    vsf_trace_info("CASE:%d" VSF_TRACE_CFG_LINEEND, (int)case_idx);
    __busy_wait_ms(MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY | VSF_USART_TX_ENABLE,
        .baudrate = baud,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_usart_enable(usart));

    __usart_send_str(usart, TEST_PAYLOAD);
    __busy_wait_ms(PAYLOAD_DRAIN_MS);
}

/*============================ TEST CASE =====================================*/

void vsf_test_usart_baud_scenario(void)
{
    uint32_t baud = (uint32_t)(uintptr_t)vsf_test_get_user_data();
    uint8_t  idx  = (uint8_t)(uintptr_t)vsf_test_get_user_data();  // same value, used as index

    // Find our index in the baudrates array to emit the correct CASE marker
    // The user_data is the baudrate value; we find which slot it occupies
    uint8_t case_idx = 0;
    if (test_usart_baudrates != NULL) {
        for (uint8_t i = 0; i < VSF_TEST_USART_BAUD_MAX_COUNT; i++) {
            if (test_usart_baudrates[i] == baud) {
                case_idx = i;
                break;
            }
        }
    }

    __run_baud_test(test_usart_instance, case_idx, baud);
}

/* EOF */
