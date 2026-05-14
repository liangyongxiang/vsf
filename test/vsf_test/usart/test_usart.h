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

//! \brief 编译开关：默认启用 baud TX 场景
#ifndef VSF_TEST_USART_TX_BAUD_ENABLE
#   define VSF_TEST_USART_TX_BAUD_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 mode TX 场景
#ifndef VSF_TEST_USART_TX_MODE_ENABLE
#   define VSF_TEST_USART_TX_MODE_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 data RX 场景
#ifndef VSF_TEST_USART_RX_DATA_ENABLE
#   define VSF_TEST_USART_RX_DATA_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 baud RX 场景
#ifndef VSF_TEST_USART_RX_BAUD_ENABLE
#   define VSF_TEST_USART_RX_BAUD_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 mode RX 场景
#ifndef VSF_TEST_USART_RX_MODE_ENABLE
#   define VSF_TEST_USART_RX_MODE_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 IRQ RX 场景
#ifndef VSF_TEST_USART_RX_IRQ_ENABLE
#   define VSF_TEST_USART_RX_IRQ_ENABLE      ENABLED
#endif

//! \brief 编译开关：默认启用 timeout RX 场景
#ifndef VSF_TEST_USART_RX_TIMEOUT_ENABLE
#   define VSF_TEST_USART_RX_TIMEOUT_ENABLE  ENABLED
#endif

//! \brief 编译开关：默认启用 parity error RX 场景
#ifndef VSF_TEST_USART_RX_PARITY_ERROR_ENABLE
#   define VSF_TEST_USART_RX_PARITY_ERROR_ENABLE  ENABLED
#endif

//! \brief 编译开关：默认启用 frame error RX 场景
#ifndef VSF_TEST_USART_RX_FRAME_ERROR_ENABLE
#   define VSF_TEST_USART_RX_FRAME_ERROR_ENABLE   ENABLED
#endif

/*============================ TYPES =========================================*/

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
//! \brief USART 波特率测试用例配置条目
typedef struct vsf_test_usart_baud_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    uint32_t baudrate;    //! \brief 目标波特率
    bool     expect_pass; //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
} vsf_test_usart_baud_case_t;
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
//! \brief USART 模式测试用例配置条目
typedef struct vsf_test_usart_mode_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
    bool             expect_pass; //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
} vsf_test_usart_mode_case_t;
#endif

//! \brief USART RX 数据测试用例配置条目
typedef struct vsf_test_usart_rx_data_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
} vsf_test_usart_rx_data_case_t;

//! \brief USART TX 测试套件配置
typedef struct vsf_test_usart_tx_cfg_t {
    //! \brief USART 实例指针
    vsf_usart_t *usart_instance;
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    //! \brief 波特率测试用例数组
    const vsf_test_usart_baud_case_t *baud_cases;
    //! \brief 波特率测试用例数量
    uint8_t baud_case_count;
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    //! \brief 模式测试用例数组
    const vsf_test_usart_mode_case_t *mode_cases;
    //! \brief 模式测试用例数量
    uint8_t mode_case_count;
#endif
} vsf_test_usart_tx_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/

//! \brief 测试使用的 USART 实例（由测试主函数设置）
extern vsf_usart_t *test_usart_instance;

//! \brief RX 测试使用的 USART 实例（由测试主函数设置）
extern vsf_usart_t *test_usart_rx_instance;

//! \brief USART RX 波特率测试用例配置条目
typedef struct vsf_test_usart_rx_baud_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    uint32_t baudrate;    //! \brief 目标波特率
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
} vsf_test_usart_rx_baud_case_t;

//! \brief USART RX 模式测试用例配置条目
typedef struct vsf_test_usart_rx_mode_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
    bool             expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
} vsf_test_usart_rx_mode_case_t;

//! \brief USART RX IRQ 测试用例配置条目
typedef struct vsf_test_usart_rx_irq_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
} vsf_test_usart_rx_irq_case_t;

//! \brief USART RX 超时测试用例配置条目
typedef struct vsf_test_usart_rx_timeout_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
} vsf_test_usart_rx_timeout_case_t;

//! \brief USART RX parity error 测试用例配置条目
typedef struct vsf_test_usart_rx_parity_error_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 parity 配置）
    bool             expect_pass; //! \brief true=预期检测到 parity error，false=预期初始化失败
} vsf_test_usart_rx_parity_error_case_t;

//! \brief USART RX frame error 测试用例配置条目
typedef struct vsf_test_usart_rx_frame_error_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 stop bit 配置）
    bool             expect_pass; //! \brief true=预期检测到 frame error，false=预期初始化失败
} vsf_test_usart_rx_frame_error_case_t;

//! \brief USART RX 测试套件配置
typedef struct vsf_test_usart_rx_cfg_t {
    //! \brief USART 实例指针
    vsf_usart_t *usart_instance;
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    //! \brief RX 数据测试用例数组
    const vsf_test_usart_rx_data_case_t *rx_data_cases;
    //! \brief RX 数据测试用例数量
    uint8_t rx_data_case_count;
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    //! \brief RX 波特率测试用例数组
    const vsf_test_usart_rx_baud_case_t *rx_baud_cases;
    //! \brief RX 波特率测试用例数量
    uint8_t rx_baud_case_count;
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    //! \brief RX 模式测试用例数组
    const vsf_test_usart_rx_mode_case_t *rx_mode_cases;
    //! \brief RX 模式测试用例数量
    uint8_t rx_mode_case_count;
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    //! \brief RX IRQ 测试用例数组
    const vsf_test_usart_rx_irq_case_t *rx_irq_cases;
    //! \brief RX IRQ 测试用例数量
    uint8_t rx_irq_case_count;
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    //! \brief RX 超时测试用例数组
    const vsf_test_usart_rx_timeout_case_t *rx_timeout_cases;
    //! \brief RX 超时测试用例数量
    uint8_t rx_timeout_case_count;
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
    //! \brief RX parity error 测试用例数组
    const vsf_test_usart_rx_parity_error_case_t *rx_parity_error_cases;
    //! \brief RX parity error 测试用例数量
    uint8_t rx_parity_error_case_count;
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
    //! \brief RX frame error 测试用例数组
    const vsf_test_usart_rx_frame_error_case_t *rx_frame_error_cases;
    //! \brief RX frame error 测试用例数量
    uint8_t rx_frame_error_case_count;
#endif
} vsf_test_usart_rx_cfg_t;

/*============================ PROTOTYPES ====================================*/

/**
 * @brief 初始化 USART TX 测试套件并注册所有启用的场景用例
 * @param cfg USART TX 测试配置
 */
void vsf_test_usart_tx_init(const vsf_test_usart_tx_cfg_t *cfg);

/**
 * @brief 初始化 USART RX 测试套件并注册所有启用的场景用例
 * @param cfg USART RX 测试配置
 */
void vsf_test_usart_rx_init(const vsf_test_usart_rx_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_USART_H__ */
/* EOF */
