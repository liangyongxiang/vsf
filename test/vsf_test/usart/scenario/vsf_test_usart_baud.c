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

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"
#include "../vsf_test_usart.h"
#include "vsf_test_usart_baud.h"

static vsf_test_usart_scenario_t s_scenario;

#include "test_params_generated.h"

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_BAUD_PAYLOAD
#   define VSF_TEST_BAUD_PAYLOAD            "Hello VSF\r\n"
#endif
#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS         200
#endif
#ifndef VSF_TEST_BAUD_PAYLOAD_DRAIN_MS
#   define VSF_TEST_BAUD_PAYLOAD_DRAIN_MS   500
#endif
#ifndef VSF_TEST_BAUD_COMMON_MODE
#   define VSF_TEST_BAUD_COMMON_MODE        (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_TX_ENABLE)
#endif

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_test_usart_baud_case_t __baud_cases[] = {
    VSF_TEST_BAUD_CASES_INIT
};

/*============================ LOCAL FUNCTIONS ===============================*/

static void __usart_send_str(vsf_usart_t *usart, const char *str)
{
    while (*str) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)str, 1);
        str++;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_baud_add_cases(vsf_usart_t *usart_instance)
{
    s_scenario.usart_instance = usart_instance;
    for (uint8_t i = 0; i < VSF_TEST_BAUD_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][64];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "usart_baud_%lu purpose=baud-rate hw_req=uart1+la",
            (unsigned long)__baud_cases[i].baudrate);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_usart_baud_run,
            __cfg_str_pool[i], (void *)&__baud_cases[i]);
    }
}

void vsf_test_usart_baud_run(const vsf_test_usart_baud_case_t *c)
{

    vsf_trace_info("BAUD:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_err_t err = vsf_usart_init(c->scenario->usart_instance, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_BAUD_COMMON_MODE,
        .baudrate = c->baudrate,
    });

    if (c->expect_pass) {
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        while (fsm_rt_cpl != vsf_usart_enable(c->scenario->usart_instance));
        __usart_send_str(c->scenario->usart_instance, VSF_TEST_BAUD_PAYLOAD);
        vsf_test_busy_wait_ms(VSF_TEST_BAUD_PAYLOAD_DRAIN_MS);
    } else {
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
}

#endif /* VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED */

/* EOF */
