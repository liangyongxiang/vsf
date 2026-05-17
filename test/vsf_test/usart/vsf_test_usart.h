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

#ifndef __VSF_TEST_USART_H__
#define __VSF_TEST_USART_H__

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

//! \brief 编译开关：TX FIFO threshold IRQ + ISR refill (gap-fill PRD)
#ifndef VSF_TEST_USART_TX_FIFO_IRQ_ENABLE
#   define VSF_TEST_USART_TX_FIFO_IRQ_ENABLE      DISABLED
#endif

//! \brief 编译开关：pure RX FIFO threshold IRQ (gap-fill PRD)
#ifndef VSF_TEST_USART_RX_FIFO_IRQ_ENABLE
#   define VSF_TEST_USART_RX_FIFO_IRQ_ENABLE      DISABLED
#endif

//! \brief 编译开关：fifo2req_usart adapter request_tx + TX_CPL IRQ (gap-fill PRD)
#ifndef VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE
#   define VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE   DISABLED
#endif

//! \brief 编译开关：fifo2req_usart adapter request_rx + RX_CPL IRQ (gap-fill PRD)
#ifndef VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE
#   define VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE   DISABLED
#endif

//! \brief 编译开关：cancel_tx/cancel_rx + get_count partial (gap-fill PRD)
#ifndef VSF_TEST_USART_REQUEST_CANCEL_ENABLE
#   define VSF_TEST_USART_REQUEST_CANCEL_ENABLE   DISABLED
#endif

/*============================ TYPES =========================================*/

// Per-scene context (populated by __vsf_test in main.c)
typedef struct vsf_test_usart_baud_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_baud_scene_t;

typedef struct vsf_test_usart_mode_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_mode_scene_t;

typedef struct vsf_test_usart_rx_data_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_data_scene_t;

typedef struct vsf_test_usart_rx_baud_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_baud_scene_t;

typedef struct vsf_test_usart_rx_mode_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_mode_scene_t;

typedef struct vsf_test_usart_rx_irq_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_irq_scene_t;

typedef struct vsf_test_usart_rx_timeout_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_timeout_scene_t;

typedef struct vsf_test_usart_rx_parity_error_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_parity_error_scene_t;

typedef struct vsf_test_usart_rx_frame_error_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_frame_error_scene_t;

typedef struct vsf_test_usart_tx_fifo_irq_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_tx_fifo_irq_scene_t;

typedef struct vsf_test_usart_rx_fifo_irq_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_rx_fifo_irq_scene_t;

typedef struct vsf_test_usart_request_tx_irq_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_request_tx_irq_scene_t;

typedef struct vsf_test_usart_request_rx_irq_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_request_rx_irq_scene_t;

typedef struct vsf_test_usart_request_cancel_scene_t {
    vsf_usart_t *usart;
} vsf_test_usart_request_cancel_scene_t;

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
//! \brief USART 波特率测试用例配置条目
typedef struct vsf_test_usart_baud_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    uint32_t baudrate;    //! \brief 目标波特率
    bool     expect_pass; //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
    vsf_test_usart_baud_scene_t *scene;
} vsf_test_usart_baud_case_t;
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
//! \brief USART 模式测试用例配置条目
typedef struct vsf_test_usart_mode_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
    bool             expect_pass; //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
    vsf_test_usart_mode_scene_t *scene;
} vsf_test_usart_mode_case_t;
#endif

//! \brief USART RX 数据测试用例配置条目
typedef struct vsf_test_usart_rx_data_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    vsf_test_usart_rx_data_scene_t *scene;
} vsf_test_usart_rx_data_case_t;

//! \brief USART RX 波特率测试用例配置条目
typedef struct vsf_test_usart_rx_baud_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    uint32_t baudrate;    //! \brief 目标波特率
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    vsf_test_usart_rx_baud_scene_t *scene;
} vsf_test_usart_rx_baud_case_t;

//! \brief USART RX 模式测试用例配置条目
typedef struct vsf_test_usart_rx_mode_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
    bool             expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    vsf_test_usart_rx_mode_scene_t *scene;
} vsf_test_usart_rx_mode_case_t;

//! \brief USART RX IRQ 测试用例配置条目
typedef struct vsf_test_usart_rx_irq_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    vsf_test_usart_rx_irq_scene_t *scene;
} vsf_test_usart_rx_irq_case_t;

//! \brief USART RX 超时测试用例配置条目
typedef struct vsf_test_usart_rx_timeout_case_t {
    uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
    bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    vsf_test_usart_rx_timeout_scene_t *scene;
} vsf_test_usart_rx_timeout_case_t;

//! \brief USART RX parity error 测试用例配置条目
typedef struct vsf_test_usart_rx_parity_error_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 parity 配置）
    bool             expect_pass; //! \brief true=预期检测到 parity error，false=预期初始化失败
    vsf_test_usart_rx_parity_error_scene_t *scene;
} vsf_test_usart_rx_parity_error_case_t;

//! \brief USART RX frame error 测试用例配置条目
typedef struct vsf_test_usart_rx_frame_error_case_t {
    uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
    vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 stop bit 配置）
    bool             expect_pass; //! \brief true=预期检测到 frame error，false=预期初始化失败
    vsf_test_usart_rx_frame_error_scene_t *scene;
} vsf_test_usart_rx_frame_error_case_t;

/* ---- Gap-fill PRD: FIFO IRQ + request API + cancel ---- */

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
typedef struct vsf_test_usart_tx_fifo_irq_case_t {
    uint8_t  idx;
    uint32_t refill_target;       //! data_size = txfifo_depth * refill_target
    vsf_test_usart_tx_fifo_irq_scene_t *scene;
} vsf_test_usart_tx_fifo_irq_case_t;
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
typedef struct vsf_test_usart_rx_fifo_irq_case_t {
    uint8_t  idx;
    uint32_t refill_target;
    vsf_test_usart_rx_fifo_irq_scene_t *scene;
} vsf_test_usart_rx_fifo_irq_case_t;
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
typedef struct vsf_test_usart_request_tx_irq_case_t {
    uint8_t  idx;
    uint32_t refill_target;
    vsf_test_usart_request_tx_irq_scene_t *scene;
} vsf_test_usart_request_tx_irq_case_t;
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
typedef struct vsf_test_usart_request_rx_irq_case_t {
    uint8_t  idx;
    uint32_t refill_target;
    vsf_test_usart_request_rx_irq_scene_t *scene;
} vsf_test_usart_request_rx_irq_case_t;
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
typedef struct vsf_test_usart_request_cancel_case_t {
    uint8_t  idx;
    uint32_t refill_target;
    uint32_t cancel_after_us;
    vsf_test_usart_request_cancel_scene_t *scene;
} vsf_test_usart_request_cancel_case_t;
#endif


typedef struct vsf_test_usart_scenes_t {
    vsf_test_usart_baud_scene_t                baud;
    vsf_test_usart_mode_scene_t                mode;
    vsf_test_usart_rx_data_scene_t             rx_data;
    vsf_test_usart_rx_baud_scene_t             rx_baud;
    vsf_test_usart_rx_mode_scene_t             rx_mode;
    vsf_test_usart_rx_irq_scene_t              rx_irq;
    vsf_test_usart_rx_timeout_scene_t          rx_timeout;
    vsf_test_usart_rx_parity_error_scene_t     rx_parity_error;
    vsf_test_usart_rx_frame_error_scene_t      rx_frame_error;
    vsf_test_usart_tx_fifo_irq_scene_t         tx_fifo_irq;
    vsf_test_usart_rx_fifo_irq_scene_t         rx_fifo_irq;
    vsf_test_usart_request_tx_irq_scene_t      request_tx_irq;
    vsf_test_usart_request_rx_irq_scene_t      request_rx_irq;
    vsf_test_usart_request_cancel_scene_t      request_cancel;
} vsf_test_usart_scenes_t;

void vsf_test_usart_register_all(vsf_test_usart_scenes_t *s);
/*============================ PROTOTYPES ====================================*/

/* ---- TX scenarios ---- */
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
void vsf_test_usart_baud_add_cases(vsf_test_usart_baud_scene_t *scene);
void vsf_test_usart_baud_run(const vsf_test_usart_baud_case_t *c);
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
void vsf_test_usart_mode_add_cases(vsf_test_usart_mode_scene_t *scene);
void vsf_test_usart_mode_run(const vsf_test_usart_mode_case_t *c);
#endif

/* ---- RX scenarios ---- */
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
void vsf_test_usart_rx_data_add_cases(vsf_test_usart_rx_data_scene_t *scene);
void vsf_test_usart_rx_data_run(const vsf_test_usart_rx_data_case_t *c);
#endif

#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
void vsf_test_usart_rx_baud_add_cases(vsf_test_usart_rx_baud_scene_t *scene);
void vsf_test_usart_rx_baud_run(const vsf_test_usart_rx_baud_case_t *c);
#endif

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
void vsf_test_usart_rx_mode_add_cases(vsf_test_usart_rx_mode_scene_t *scene);
void vsf_test_usart_rx_mode_run(const vsf_test_usart_rx_mode_case_t *c);
#endif

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_irq_add_cases(vsf_test_usart_rx_irq_scene_t *scene);
void vsf_test_usart_rx_irq_run(const vsf_test_usart_rx_irq_case_t *c);
#endif

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
void vsf_test_usart_rx_timeout_add_cases(vsf_test_usart_rx_timeout_scene_t *scene);
void vsf_test_usart_rx_timeout_run(const vsf_test_usart_rx_timeout_case_t *c);
#endif

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_add_cases(vsf_test_usart_rx_parity_error_scene_t *scene);
void vsf_test_usart_rx_parity_error_run(const vsf_test_usart_rx_parity_error_case_t *c);
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_add_cases(vsf_test_usart_rx_frame_error_scene_t *scene);
void vsf_test_usart_rx_frame_error_run(const vsf_test_usart_rx_frame_error_case_t *c);
#endif

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
void vsf_test_usart_tx_fifo_irq_add_cases(vsf_test_usart_tx_fifo_irq_scene_t *scene);
void vsf_test_usart_tx_fifo_irq_run(const vsf_test_usart_tx_fifo_irq_case_t *c);
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_fifo_irq_add_cases(vsf_test_usart_rx_fifo_irq_scene_t *scene);
void vsf_test_usart_rx_fifo_irq_run(const vsf_test_usart_rx_fifo_irq_case_t *c);
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
void vsf_test_usart_request_tx_irq_add_cases(vsf_test_usart_request_tx_irq_scene_t *scene);
void vsf_test_usart_request_tx_irq_run(const vsf_test_usart_request_tx_irq_case_t *c);
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
void vsf_test_usart_request_rx_irq_add_cases(vsf_test_usart_request_rx_irq_scene_t *scene);
void vsf_test_usart_request_rx_irq_run(const vsf_test_usart_request_rx_irq_case_t *c);
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
void vsf_test_usart_request_cancel_add_cases(vsf_test_usart_request_cancel_scene_t *scene);
void vsf_test_usart_request_cancel_run(const vsf_test_usart_request_cancel_case_t *c);
#endif

#include "test_params_generated.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_USART_H__ */
/* EOF */
