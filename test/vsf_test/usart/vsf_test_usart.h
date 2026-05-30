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

#if     defined(__VSF_TEST_USART_CLASS_IMPLEMENT)
#   undef __VSF_TEST_USART_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#   include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

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
#ifndef VSF_TEST_USART_RX_BREAK_ERROR_ENABLE
#   define VSF_TEST_USART_RX_BREAK_ERROR_ENABLE   DISABLED
#endif
#ifndef VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE
#   define VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE DISABLED
#endif
#ifndef VSF_TEST_USART_BREAK_SIGNAL_ENABLE
#   define VSF_TEST_USART_BREAK_SIGNAL_ENABLE     DISABLED
#endif
#ifndef VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE
#   define VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE  DISABLED
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

#ifndef VSF_TEST_USART_RX_BULK_IRQ_ENABLE
#   define VSF_TEST_USART_RX_BULK_IRQ_ENABLE      DISABLED
#endif

#ifndef VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE
#   define VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE DISABLED
#endif

/*============================ TYPES =========================================*/

// Per-suite context (populated by __vsf_test in main.c)



















#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
#endif

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
//! \brief USART 波特率测试用例配置条目
vsf_class(vsf_test_usart_baud_params_t) {
    public_member(
        uint8_t  idx;              //! \brief 场景内索引，用于 CASE:marker
        uint32_t baudrate;         //! \brief 目标波特率
        uint32_t data_size_bytes;  //! \brief 0=使用字符串payload，>0=使用递增计数器pattern
        bool     expect_pass;      //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
    )
};
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
//! \brief USART 模式测试用例配置条目
vsf_class(vsf_test_usart_mode_params_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
        bool             expect_pass; //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
    )
};
#endif

//! \brief USART RX 数据测试用例配置条目
vsf_class(vsf_test_usart_rx_data_params_t) {
    public_member(
        uint8_t  idx;              //! \brief 场景内索引，用于 CASE:marker
        uint32_t data_size_bytes;  //! \brief 0=使用字符串payload，>0=使用递增计数器pattern
        bool     expect_pass;      //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    )
};

//! \brief USART RX 波特率测试用例配置条目
vsf_class(vsf_test_usart_rx_baud_params_t) {
    public_member(
        uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
        uint32_t baudrate;    //! \brief 目标波特率
        bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    )
};

//! \brief USART RX 模式测试用例配置条目
vsf_class(vsf_test_usart_rx_mode_params_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
        bool             expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    )
};

//! \brief USART RX IRQ 测试用例配置条目
vsf_class(vsf_test_usart_rx_irq_params_t) {
    public_member(
        uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
        bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    )
};

//! \brief USART RX 超时测试用例配置条目
vsf_class(vsf_test_usart_rx_timeout_params_t) {
    public_member(
        uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
        bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
    )
};

//! \brief USART RX parity error 测试用例配置条目
vsf_class(vsf_test_usart_rx_parity_error_params_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 parity 配置）
        bool             expect_pass; //! \brief true=预期检测到 parity error，false=预期初始化失败
    )
};

//! \brief USART RX frame error 测试用例配置条目
vsf_class(vsf_test_usart_rx_frame_error_params_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 stop bit 配置）
        bool             expect_pass; //! \brief true=预期检测到 frame error，false=预期初始化失败
    )
};

//! \brief USART RX break error 测试用例配置条目
vsf_class(vsf_test_usart_rx_break_error_params_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t mode;
        bool             expect_pass;
    )
};

//! \brief USART RX overflow error 测试用例配置条目
vsf_class(vsf_test_usart_rx_overflow_error_params_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t mode;
        bool             expect_pass;
    )
};

//! \brief USART TX break signal 测试用例配置条目
vsf_class(vsf_test_usart_break_signal_params_t) {
    public_member(
        uint8_t  idx;
        uint32_t baudrate;
        uint32_t hold_ms;        //! \brief SET_BREAK hold duration in ms
    )
};

//! \brief USART hardware flow control 测试用例配置条目
vsf_class(vsf_test_usart_hw_flow_control_params_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t flow_mode;     //! one of VSF_USART_RTS_HWCONTROL etc.
    )
};

/* ---- Gap-fill PRD: FIFO IRQ + request API + cancel ---- */

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_tx_fifo_irq_params_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;       //! data_size = txfifo_depth * refill_target
    )
};
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_fifo_irq_params_t) {
    public_member(
        uint8_t          idx;
        uint32_t         refill_target;
        vsf_usart_mode_t threshold_mode;    //! one of VSF_USART_RX_FIFO_THRESHOLD_*
    )
};
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_request_tx_irq_params_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;
    )
};
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_request_rx_irq_params_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;
    )
};
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
vsf_class(vsf_test_usart_request_cancel_params_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;
        uint32_t cancel_after_us;
    )
};
#endif

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_bulk_irq_params_t) {
    public_member(
        uint8_t  idx;
        uint32_t data_size_bytes;
    )
};
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_fifo_threshold_params_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t threshold_mode;    //! one of VSF_USART_RX_FIFO_THRESHOLD_*
        uint32_t         expected_bytes;    //! expected bytes when threshold IRQ fires
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

/* ---- TX suites ---- */
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
void vsf_test_usart_baud_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
void vsf_test_usart_mode_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

/* ---- RX suites ---- */
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
void vsf_test_usart_rx_data_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
void vsf_test_usart_rx_baud_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
void vsf_test_usart_rx_mode_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
void vsf_test_usart_rx_timeout_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_break_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_overflow_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
void vsf_test_usart_break_signal_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
void vsf_test_usart_hw_flow_control_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
void vsf_test_usart_tx_fifo_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_fifo_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
void vsf_test_usart_request_tx_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
void vsf_test_usart_request_rx_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
void vsf_test_usart_request_cancel_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_bulk_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
void vsf_test_usart_rx_fifo_threshold_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

// Framework types — included LAST so this header can be pulled into
// vsf_test.h without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif


/*============================ SUITE TABLE ==================================*/

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
#   define __vsf_test_usart_request_cancel_suite { .name = "usart_request_cancel", .cases = __usart_request_cancel_cases, .case_count = dimof(__usart_request_cancel_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_request_cancel_suite
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
#   define __vsf_test_usart_request_rx_irq_suite { .name = "usart_request_rx_irq", .cases = __usart_request_rx_irq_cases, .case_count = dimof(__usart_request_rx_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_request_rx_irq_suite
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
#   define __vsf_test_usart_request_tx_irq_suite { .name = "usart_request_tx_irq", .cases = __usart_request_tx_irq_cases, .case_count = dimof(__usart_request_tx_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_request_tx_irq_suite
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
#   define __vsf_test_usart_rx_baud_suite { .name = "usart_rx_baud", .cases = __usart_rx_baud_cases, .case_count = dimof(__usart_rx_baud_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_baud_suite
#endif
#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
#   define __vsf_test_usart_rx_break_error_suite { .name = "usart_rx_break_error", .cases = __usart_rx_break_error_cases, .case_count = dimof(__usart_rx_break_error_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_break_error_suite
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
#   define __vsf_test_usart_rx_bulk_irq_suite { .name = "usart_rx_bulk_irq", .cases = __usart_rx_bulk_irq_cases, .case_count = dimof(__usart_rx_bulk_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_bulk_irq_suite
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
#   define __vsf_test_usart_rx_data_suite { .name = "usart_rx_data", .cases = __usart_rx_data_cases, .case_count = dimof(__usart_rx_data_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_data_suite
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
#   define __vsf_test_usart_rx_fifo_irq_suite { .name = "usart_rx_fifo_irq", .cases = __usart_rx_fifo_irq_cases, .case_count = dimof(__usart_rx_fifo_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_fifo_irq_suite
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
#   define __vsf_test_usart_rx_fifo_threshold_suite { .name = "usart_rx_fifo_threshold", .cases = __usart_rx_fifo_threshold_cases, .case_count = dimof(__usart_rx_fifo_threshold_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_fifo_threshold_suite
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
#   define __vsf_test_usart_rx_frame_error_suite { .name = "usart_rx_frame_error", .cases = __usart_rx_frame_error_cases, .case_count = dimof(__usart_rx_frame_error_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_frame_error_suite
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
#   define __vsf_test_usart_rx_irq_suite { .name = "usart_rx_irq", .cases = __usart_rx_irq_cases, .case_count = dimof(__usart_rx_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_irq_suite
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
#   define __vsf_test_usart_rx_mode_suite { .name = "usart_rx_mode", .cases = __usart_rx_mode_cases, .case_count = dimof(__usart_rx_mode_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_mode_suite
#endif
#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
#   define __vsf_test_usart_rx_overflow_error_suite { .name = "usart_rx_overflow_error", .cases = __usart_rx_overflow_error_cases, .case_count = dimof(__usart_rx_overflow_error_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_overflow_error_suite
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
#   define __vsf_test_usart_rx_parity_error_suite { .name = "usart_rx_parity_error", .cases = __usart_rx_parity_error_cases, .case_count = dimof(__usart_rx_parity_error_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_parity_error_suite
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
#   define __vsf_test_usart_rx_timeout_suite { .name = "usart_rx_timeout", .cases = __usart_rx_timeout_cases, .case_count = dimof(__usart_rx_timeout_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_rx_timeout_suite
#endif
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
#   define __vsf_test_usart_baud_suite { .name = "usart_baud", .cases = __usart_baud_cases, .case_count = dimof(__usart_baud_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_baud_suite
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
#   define __vsf_test_usart_tx_fifo_irq_suite { .name = "usart_tx_fifo_irq", .cases = __usart_tx_fifo_irq_cases, .case_count = dimof(__usart_tx_fifo_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_tx_fifo_irq_suite
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
#   define __vsf_test_usart_mode_suite { .name = "usart_mode", .cases = __usart_mode_cases, .case_count = dimof(__usart_mode_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_mode_suite
#endif
#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
#   define __vsf_test_usart_break_signal_suite { .name = "usart_break_signal", .cases = __usart_break_signal_cases, .case_count = dimof(__usart_break_signal_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_break_signal_suite
#endif
#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
#   define __vsf_test_usart_hw_flow_control_suite { .name = "usart_hw_flow_control", .cases = __usart_hw_flow_control_cases, .case_count = dimof(__usart_hw_flow_control_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_USART },
#else
#   define __vsf_test_usart_hw_flow_control_suite
#endif

#define VSF_TEST_USART_SUITES \
    __vsf_test_usart_request_cancel_suite \
    __vsf_test_usart_request_rx_irq_suite \
    __vsf_test_usart_request_tx_irq_suite \
    __vsf_test_usart_rx_baud_suite \
    __vsf_test_usart_rx_break_error_suite \
    __vsf_test_usart_rx_bulk_irq_suite \
    __vsf_test_usart_rx_data_suite \
    __vsf_test_usart_rx_fifo_irq_suite \
    __vsf_test_usart_rx_fifo_threshold_suite \
    __vsf_test_usart_rx_frame_error_suite \
    __vsf_test_usart_rx_irq_suite \
    __vsf_test_usart_rx_mode_suite \
    __vsf_test_usart_rx_overflow_error_suite \
    __vsf_test_usart_rx_parity_error_suite \
    __vsf_test_usart_rx_timeout_suite \
    __vsf_test_usart_baud_suite \
    __vsf_test_usart_tx_fifo_irq_suite \
    __vsf_test_usart_mode_suite \
    __vsf_test_usart_break_signal_suite \
    __vsf_test_usart_hw_flow_control_suite

#include "suite/vsf_test_usart_request_rx_irq.h"
#include "suite/vsf_test_usart_request_tx_irq.h"
#include "suite/vsf_test_usart_rx_bulk_irq.h"
#include "suite/vsf_test_usart_rx_data.h"
#include "suite/vsf_test_usart_rx_fifo_irq.h"
#include "suite/vsf_test_usart_rx_fifo_threshold.h"
#include "suite/vsf_test_usart_tx_fifo_irq.h"

#endif /* __VSF_TEST_USART_H__ */
/* EOF */
