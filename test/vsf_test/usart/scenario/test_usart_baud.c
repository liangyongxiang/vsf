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

/*============================ GENERIC TEST FUNCTIONS ========================*/

#define __TEST_BAUD_FN(__IDX)                                               \
    static void __vsf_test_usart_baud_##__IDX(void)                         \
    {                                                                       \
        __run_baud_test(test_usart_instance, __IDX,                         \
                         test_usart_baudrates[__IDX]);                      \
    }

__TEST_BAUD_FN(0)
__TEST_BAUD_FN(1)
__TEST_BAUD_FN(2)
__TEST_BAUD_FN(3)
__TEST_BAUD_FN(4)
__TEST_BAUD_FN(5)
__TEST_BAUD_FN(6)
__TEST_BAUD_FN(7)
__TEST_BAUD_FN(8)
__TEST_BAUD_FN(9)
__TEST_BAUD_FN(10)
__TEST_BAUD_FN(11)
__TEST_BAUD_FN(12)
__TEST_BAUD_FN(13)
__TEST_BAUD_FN(14)
__TEST_BAUD_FN(15)

const vsf_test_usart_baud_fn_t vsf_test_usart_baud_scenarios[VSF_TEST_USART_BAUD_MAX_COUNT] = {
    __vsf_test_usart_baud_0,
    __vsf_test_usart_baud_1,
    __vsf_test_usart_baud_2,
    __vsf_test_usart_baud_3,
    __vsf_test_usart_baud_4,
    __vsf_test_usart_baud_5,
    __vsf_test_usart_baud_6,
    __vsf_test_usart_baud_7,
    __vsf_test_usart_baud_8,
    __vsf_test_usart_baud_9,
    __vsf_test_usart_baud_10,
    __vsf_test_usart_baud_11,
    __vsf_test_usart_baud_12,
    __vsf_test_usart_baud_13,
    __vsf_test_usart_baud_14,
    __vsf_test_usart_baud_15,
};

/* EOF */
