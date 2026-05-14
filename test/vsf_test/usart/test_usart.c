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

#include "component/test/vsf_test/vsf_test.h"
#include "test_usart.h"

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
#   include "scenario/test_usart_baud.h"
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
#   include "scenario/test_usart_mode.h"
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
#   include "scenario/test_usart_rx_data.h"
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

vsf_usart_t *test_usart_instance = NULL;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 初始化 USART TX 测试套件并注册所有启用的场景用例
 * @param cfg USART TX 测试配置
 */
void vsf_test_usart_tx_init(const vsf_test_usart_tx_cfg_t *cfg)
{
    VSF_ASSERT(cfg != NULL);
    test_usart_instance = cfg->usart_instance;

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    VSF_ASSERT(cfg->baud_cases != NULL);
    VSF_ASSERT(cfg->baud_case_count > 0);
    VSF_ASSERT(cfg->baud_case_count <= VSF_TEST_USART_CASE_MAX_COUNT);

    static char __baud_cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][64];
    for (uint8_t i = 0; i < cfg->baud_case_count; i++) {
        snprintf(__baud_cfg_str_pool[i], sizeof(__baud_cfg_str_pool[i]),
            "usart_baud_%lu purpose=baud-rate hw_req=uart1+la",
            (unsigned long)cfg->baud_cases[i].baudrate);
        vsf_test_add_simple_case(vsf_test_usart_baud_scenario,
            __baud_cfg_str_pool[i], (void *)&cfg->baud_cases[i]);
    }
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    VSF_ASSERT(cfg->mode_cases != NULL);
    VSF_ASSERT(cfg->mode_case_count > 0);
    VSF_ASSERT(cfg->mode_case_count <= VSF_TEST_USART_CASE_MAX_COUNT);

    static char __mode_cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][64];
    for (uint8_t i = 0; i < cfg->mode_case_count; i++) {
        snprintf(__mode_cfg_str_pool[i], sizeof(__mode_cfg_str_pool[i]),
            "usart_mode_%u purpose=mode hw_req=uart1+la",
            (unsigned)cfg->mode_cases[i].idx);
        vsf_test_add_simple_case(vsf_test_usart_mode_scenario,
            __mode_cfg_str_pool[i], (void *)&cfg->mode_cases[i]);
    }
#endif
}

/**
 * @brief 初始化 USART RX 测试套件并注册所有启用的场景用例
 * @param cfg USART RX 测试配置
 */
void vsf_test_usart_rx_init(const vsf_test_usart_rx_cfg_t *cfg)
{
    VSF_ASSERT(cfg != NULL);
    test_usart_instance = cfg->usart_instance;

#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    VSF_ASSERT(cfg->rx_data_cases != NULL);
    VSF_ASSERT(cfg->rx_data_case_count > 0);
    VSF_ASSERT(cfg->rx_data_case_count <= VSF_TEST_USART_CASE_MAX_COUNT);

    static char __rx_data_cfg_str_pool[VSF_TEST_USART_CASE_MAX_COUNT][64];
    for (uint8_t i = 0; i < cfg->rx_data_case_count; i++) {
        snprintf(__rx_data_cfg_str_pool[i], sizeof(__rx_data_cfg_str_pool[i]),
            "usart_rx_data_%u purpose=rx-data hw_req=uart1+la",
            (unsigned)cfg->rx_data_cases[i].idx);
        vsf_test_add_simple_case(vsf_test_usart_rx_data_scenario,
            __rx_data_cfg_str_pool[i], (void *)&cfg->rx_data_cases[i]);
    }
#endif
}

/* EOF */
