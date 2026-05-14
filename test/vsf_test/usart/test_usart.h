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

#ifndef __TEST_USART_H__
#define __TEST_USART_H__

/*============================ INCLUDES ======================================*/

#   include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

//! \brief 最大支持的波特率测试数量
#define VSF_TEST_USART_BAUD_MAX_COUNT   16

/*============================ TYPES =========================================*/

//! \brief USART 测试配置结构
typedef struct vsf_test_usart_cfg_t {
    //! \brief USART 实例指针，用于测试
    vsf_usart_t *usart_instance;
    //! \brief 波特率数组，NULL 表示使用默认波特率
    const uint32_t *baudrates;
    //! \brief 波特率数量，0 表示使用默认 8 个
    uint8_t baud_count;
} vsf_test_usart_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/

//! \brief 测试使用的 USART 实例（由测试主函数设置）
extern vsf_usart_t *test_usart_instance;

//! \brief 当前波特率测试数组（由 vsf_test_usart_init 设置）
extern const uint32_t *test_usart_baudrates;

/*============================ PROTOTYPES ====================================*/

/**
 * @brief 初始化 USART 测试并添加测试用例
 * @param cfg USART 测试配置，包含 USART 实例指针和可选波特率数组
 */
void vsf_test_usart_init(const vsf_test_usart_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_USART_H__ */
/* EOF */
