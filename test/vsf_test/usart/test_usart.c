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
#include "scenario/test_usart_baud.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

vsf_usart_t *test_usart_instance = NULL;

/*============================ LOCAL VARIABLES ===============================*/

static const uint32_t __default_baudrates[] = {
    9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 初始化 USART 测试并添加测试用例
 * @param cfg USART 测试配置，包含 USART 实例指针和可选波特率数组
 */
void vsf_test_usart_init(const vsf_test_usart_cfg_t *cfg)
{
    VSF_ASSERT(cfg != NULL);
    VSF_ASSERT(cfg->baud_count <= VSF_TEST_USART_BAUD_MAX_COUNT);

    // 设置 USART 测试实例
    test_usart_instance = cfg->usart_instance;

    // 确定波特率数组和数量
    const uint32_t *baudrates = cfg->baudrates;
    uint8_t baud_count = cfg->baud_count;
    if (baudrates == NULL || baud_count == 0) {
        baudrates = __default_baudrates;
        baud_count = sizeof(__default_baudrates) / sizeof(__default_baudrates[0]);
    }
    test_usart_baudrates = baudrates;

    // 场景测试：波特率精度
    for (uint8_t i = 0; i < baud_count; i++) {
        char cfg_str[64];
        snprintf(cfg_str, sizeof(cfg_str),
            "usart_baud_%lu purpose=baud-rate hw_req=uart1+la", (unsigned long)baudrates[i]);
        vsf_test_add_simple_case(vsf_test_usart_baud_scenarios[i], cfg_str);
    }
}

/* EOF */
