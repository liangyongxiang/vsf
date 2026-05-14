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

//! \brief 最大支持的用例数量
#define VSF_TEST_USART_CASE_MAX_COUNT   16

/*============================ TYPES =========================================*/

//! \brief USART 波特率测试用例配置条目
typedef struct vsf_test_usart_baud_case_t {
    uint8_t  idx;       //! \brief 场景内索引，用于 CASE:marker
    uint32_t baudrate;  //! \brief 目标波特率
} vsf_test_usart_baud_case_t;

//! \brief USART 测试套件配置
typedef struct vsf_test_usart_cfg_t {
    //! \brief USART 实例指针
    vsf_usart_t *usart_instance;
    //! \brief 波特率测试用例配置数组，NULL 表示使用默认配置
    const vsf_test_usart_baud_case_t *baud_cases;
    //! \brief 波特率测试用例数量，0 表示使用默认配置
    uint8_t baud_case_count;
} vsf_test_usart_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/

//! \brief 测试使用的 USART 实例（由测试主函数设置）
extern vsf_usart_t *test_usart_instance;

/*============================ PROTOTYPES ====================================*/

/**
 * @brief 初始化 USART 测试并添加测试用例
 * @param cfg USART 测试配置
 */
void vsf_test_usart_init(const vsf_test_usart_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_USART_H__ */
/* EOF */
