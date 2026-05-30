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
vsf_class(vsf_test_usart_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_usart_t *usart;
    )
};

vsf_class(vsf_test_usart_baud_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_mode_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_data_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_baud_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_mode_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_irq_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_timeout_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_parity_error_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_frame_error_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_break_error_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_rx_overflow_error_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_break_signal_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

vsf_class(vsf_test_usart_hw_flow_control_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
    private_member(
        volatile uint32_t cts_count;
    )
};

vsf_class(vsf_test_usart_tx_fifo_irq_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
    private_member(
        /* Per-case mutable state (run() MUST re-initialise before each case). */
        const uint8_t    *src;
        uint32_t          remaining;
        volatile uint32_t isr_count;
        volatile bool     done;
    )
};

vsf_class(vsf_test_usart_rx_fifo_irq_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
    private_member(
        /* Per-case mutable state (run() MUST re-initialise before each case). */
        uint8_t  *dst;
        uint32_t  received;
        uint32_t  target;
        volatile uint32_t isr_count;
        volatile bool done;
    )
};

vsf_class(vsf_test_usart_request_tx_irq_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
    private_member(
        /* Per-case mutable state (run() MUST re-initialise before each case). */
        volatile bool     req_tx_cpl;
        volatile uint32_t req_tx_irq_count;
    )
};

vsf_class(vsf_test_usart_request_rx_irq_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
    private_member(
        /* Per-case mutable state (run() MUST re-initialise before each case). */
        volatile bool     req_rx_cpl;
        volatile uint32_t req_rx_irq_count;
        uint8_t  req_rx_buf[256];
        uint8_t  req_rx_txbuf[256];
    )
};

vsf_class(vsf_test_usart_request_cancel_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
};

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_bulk_irq_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
    private_member(
        uint8_t  *dst;
        uint32_t  target;
        volatile uint32_t received;
        volatile uint32_t isr_count;
        volatile bool done;
    )
};
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_fifo_threshold_suite_t) {
    public_member(
        implement(vsf_test_usart_suite_base_t)
    )
    private_member(
        uint8_t  *dst;
        uint32_t  target;
        volatile uint32_t received;
        volatile uint32_t isr_count;
        volatile bool threshold_fired;
        volatile uint32_t bytes_at_threshold;
        volatile bool done;
    )
};
#endif

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
//! \brief USART 波特率测试用例配置条目
vsf_class(vsf_test_usart_baud_case_t) {
    public_member(
        uint8_t  idx;              //! \brief 场景内索引，用于 CASE:marker
        uint32_t baudrate;         //! \brief 目标波特率
        uint32_t data_size_bytes;  //! \brief 0=使用字符串payload，>0=使用递增计数器pattern
        bool     expect_pass;      //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
        vsf_test_usart_baud_suite_t *suite;
    )
};
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
//! \brief USART 模式测试用例配置条目
vsf_class(vsf_test_usart_mode_case_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
        bool             expect_pass; //! \brief true=预期初始化成功并发送数据，false=预期初始化失败
        vsf_test_usart_mode_suite_t *suite;
    )
};
#endif

//! \brief USART RX 数据测试用例配置条目
vsf_class(vsf_test_usart_rx_data_case_t) {
    public_member(
        uint8_t  idx;              //! \brief 场景内索引，用于 CASE:marker
        uint32_t data_size_bytes;  //! \brief 0=使用字符串payload，>0=使用递增计数器pattern
        bool     expect_pass;      //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
        vsf_test_usart_rx_data_suite_t *suite;
    )
};

//! \brief USART RX 波特率测试用例配置条目
vsf_class(vsf_test_usart_rx_baud_case_t) {
    public_member(
        uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
        uint32_t baudrate;    //! \brief 目标波特率
        bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
        vsf_test_usart_rx_baud_suite_t *suite;
    )
};

//! \brief USART RX 模式测试用例配置条目
vsf_class(vsf_test_usart_rx_mode_case_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（parity/stop/data/...）
        bool             expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
        vsf_test_usart_rx_mode_suite_t *suite;
    )
};

//! \brief USART RX IRQ 测试用例配置条目
vsf_class(vsf_test_usart_rx_irq_case_t) {
    public_member(
        uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
        bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
        vsf_test_usart_rx_irq_suite_t *suite;
    )
};

//! \brief USART RX 超时测试用例配置条目
vsf_class(vsf_test_usart_rx_timeout_case_t) {
    public_member(
        uint8_t  idx;         //! \brief 场景内索引，用于 CASE:marker
        bool     expect_pass; //! \brief true=预期初始化成功并接收数据，false=预期初始化失败
        vsf_test_usart_rx_timeout_suite_t *suite;
    )
};

//! \brief USART RX parity error 测试用例配置条目
vsf_class(vsf_test_usart_rx_parity_error_case_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 parity 配置）
        bool             expect_pass; //! \brief true=预期检测到 parity error，false=预期初始化失败
        vsf_test_usart_rx_parity_error_suite_t *suite;
    )
};

//! \brief USART RX frame error 测试用例配置条目
vsf_class(vsf_test_usart_rx_frame_error_case_t) {
    public_member(
        uint8_t          idx;         //! \brief 场景内索引，用于 CASE:marker
        vsf_usart_mode_t mode;        //! \brief USART 模式位掩码（含 stop bit 配置）
        bool             expect_pass; //! \brief true=预期检测到 frame error，false=预期初始化失败
        vsf_test_usart_rx_frame_error_suite_t *suite;
    )
};

//! \brief USART RX break error 测试用例配置条目
vsf_class(vsf_test_usart_rx_break_error_case_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t mode;
        bool             expect_pass;
        vsf_test_usart_rx_break_error_suite_t *suite;
    )
};

//! \brief USART RX overflow error 测试用例配置条目
vsf_class(vsf_test_usart_rx_overflow_error_case_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t mode;
        bool             expect_pass;
        vsf_test_usart_rx_overflow_error_suite_t *suite;
    )
};

//! \brief USART TX break signal 测试用例配置条目
vsf_class(vsf_test_usart_break_signal_case_t) {
    public_member(
        uint8_t  idx;
        uint32_t baudrate;
        uint32_t hold_ms;        //! \brief SET_BREAK hold duration in ms
        vsf_test_usart_break_signal_suite_t *suite;
    )
};

//! \brief USART hardware flow control 测试用例配置条目
vsf_class(vsf_test_usart_hw_flow_control_case_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t flow_mode;     //! one of VSF_USART_RTS_HWCONTROL etc.
        vsf_test_usart_hw_flow_control_suite_t *suite;
    )
};

/* ---- Gap-fill PRD: FIFO IRQ + request API + cancel ---- */

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_tx_fifo_irq_case_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;       //! data_size = txfifo_depth * refill_target
        vsf_test_usart_tx_fifo_irq_suite_t *suite;
    )
};
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_fifo_irq_case_t) {
    public_member(
        uint8_t          idx;
        uint32_t         refill_target;
        vsf_usart_mode_t threshold_mode;    //! one of VSF_USART_RX_FIFO_THRESHOLD_*
        vsf_test_usart_rx_fifo_irq_suite_t *suite;
    )
};
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_request_tx_irq_case_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;
        vsf_test_usart_request_tx_irq_suite_t *suite;
    )
};
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_request_rx_irq_case_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;
        vsf_test_usart_request_rx_irq_suite_t *suite;
    )
};
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
vsf_class(vsf_test_usart_request_cancel_case_t) {
    public_member(
        uint8_t  idx;
        uint32_t refill_target;
        uint32_t cancel_after_us;
        vsf_test_usart_request_cancel_suite_t *suite;
    )
};
#endif

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_bulk_irq_case_t) {
    public_member(
        uint8_t  idx;
        uint32_t data_size_bytes;
        vsf_test_usart_rx_bulk_irq_suite_t *suite;
    )
};
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
vsf_class(vsf_test_usart_rx_fifo_threshold_case_t) {
    public_member(
        uint8_t          idx;
        vsf_usart_mode_t threshold_mode;    //! one of VSF_USART_RX_FIFO_THRESHOLD_*
        uint32_t         expected_bytes;    //! expected bytes when threshold IRQ fires
        vsf_test_usart_rx_fifo_threshold_suite_t *suite;
    )
};
#endif

/*============================ STATIC INIT MACROS ============================*/

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
#define VSF_TEST_USART_BAUD_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_baud_suite_t suite_var; \
    static vsf_test_usart_baud_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_TX_BAUD_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_TX_BAUD_CASES(__##suite_var##_data, vsf_test_usart_baud_run, false) \
    }; \
    static vsf_test_usart_baud_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "tx-baud", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
#define VSF_TEST_USART_MODE_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_mode_suite_t suite_var; \
    static vsf_test_usart_mode_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_TX_MODE_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_TX_MODE_CASES(__##suite_var##_data, vsf_test_usart_mode_run, false) \
    }; \
    static vsf_test_usart_mode_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "tx-mode", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
#define VSF_TEST_USART_RX_DATA_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_data_suite_t suite_var; \
    static vsf_test_usart_rx_data_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_DATA_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_DATA_CASES(__##suite_var##_data, vsf_test_usart_rx_data_run, true) \
    }; \
    static vsf_test_usart_rx_data_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-data", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
#define VSF_TEST_USART_RX_BAUD_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_baud_suite_t suite_var; \
    static vsf_test_usart_rx_baud_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_BAUD_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_BAUD_CASES(__##suite_var##_data, vsf_test_usart_rx_baud_run, true) \
    }; \
    static vsf_test_usart_rx_baud_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-baud", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
#define VSF_TEST_USART_RX_MODE_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_mode_suite_t suite_var; \
    static vsf_test_usart_rx_mode_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_MODE_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_MODE_CASES(__##suite_var##_data, vsf_test_usart_rx_mode_run, true) \
    }; \
    static vsf_test_usart_rx_mode_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-mode", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
#define VSF_TEST_USART_RX_IRQ_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_irq_suite_t suite_var; \
    static vsf_test_usart_rx_irq_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_IRQ_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_IRQ_CASES(__##suite_var##_data, vsf_test_usart_rx_irq_run, true) \
    }; \
    static vsf_test_usart_rx_irq_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-irq", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
#define VSF_TEST_USART_RX_TIMEOUT_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_timeout_suite_t suite_var; \
    static vsf_test_usart_rx_timeout_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_TIMEOUT_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_TIMEOUT_CASES(__##suite_var##_data, vsf_test_usart_rx_timeout_run, true) \
    }; \
    static vsf_test_usart_rx_timeout_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-timeout", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
#define VSF_TEST_USART_RX_PARITY_ERROR_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_parity_error_suite_t suite_var; \
    static vsf_test_usart_rx_parity_error_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_PARITY_ERROR_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_PARITY_ERROR_CASES(__##suite_var##_data, vsf_test_usart_rx_parity_error_run, true) \
    }; \
    static vsf_test_usart_rx_parity_error_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-parity", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
#define VSF_TEST_USART_RX_FRAME_ERROR_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_frame_error_suite_t suite_var; \
    static vsf_test_usart_rx_frame_error_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_FRAME_ERROR_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_FRAME_ERROR_CASES(__##suite_var##_data, vsf_test_usart_rx_frame_error_run, true) \
    }; \
    static vsf_test_usart_rx_frame_error_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-frame", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
#define VSF_TEST_USART_RX_BREAK_ERROR_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_break_error_suite_t suite_var; \
    static vsf_test_usart_rx_break_error_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_BREAK_ERROR_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_BREAK_ERROR_CASES(__##suite_var##_data, vsf_test_usart_rx_break_error_run, true) \
    }; \
    static vsf_test_usart_rx_break_error_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-break", \
        .hw_req     = "uart1+host", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
#define VSF_TEST_USART_RX_OVERFLOW_ERROR_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_overflow_error_suite_t suite_var; \
    static vsf_test_usart_rx_overflow_error_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_OVERFLOW_ERROR_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_OVERFLOW_ERROR_CASES(__##suite_var##_data, vsf_test_usart_rx_overflow_error_run, true) \
    }; \
    static vsf_test_usart_rx_overflow_error_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-overflow", \
        .hw_req     = "uart1+host", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
#define VSF_TEST_USART_BREAK_SIGNAL_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_break_signal_suite_t suite_var; \
    static vsf_test_usart_break_signal_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_BREAK_SIGNAL_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_BREAK_SIGNAL_CASES(__##suite_var##_data, vsf_test_usart_break_signal_run, false) \
    }; \
    static vsf_test_usart_break_signal_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "tx-break", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
#define VSF_TEST_USART_HW_FLOW_CONTROL_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_hw_flow_control_suite_t suite_var; \
    static vsf_test_usart_hw_flow_control_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_HW_FLOW_CONTROL_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_HW_FLOW_CONTROL_CASES(__##suite_var##_data, vsf_test_usart_hw_flow_control_run, false) \
    }; \
    static vsf_test_usart_hw_flow_control_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rts-cts", \
        .hw_req     = "uart1", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
#define VSF_TEST_USART_TX_FIFO_IRQ_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_tx_fifo_irq_suite_t suite_var; \
    static vsf_test_usart_tx_fifo_irq_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_TX_FIFO_IRQ_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_TX_FIFO_IRQ_CASES(__##suite_var##_data, vsf_test_usart_tx_fifo_irq_run, true) \
    }; \
    static vsf_test_usart_tx_fifo_irq_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "tx-fifo-irq", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
#define VSF_TEST_USART_RX_FIFO_IRQ_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_fifo_irq_suite_t suite_var; \
    static vsf_test_usart_rx_fifo_irq_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_FIFO_IRQ_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_FIFO_IRQ_CASES(__##suite_var##_data, vsf_test_usart_rx_fifo_irq_run, true) \
    }; \
    static vsf_test_usart_rx_fifo_irq_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-fifo-irq", \
        .hw_req     = "uart1+la+host_send", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
#define VSF_TEST_USART_REQUEST_TX_IRQ_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_request_tx_irq_suite_t suite_var; \
    static vsf_test_usart_request_tx_irq_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_REQUEST_TX_IRQ_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_REQUEST_TX_IRQ_CASES(__##suite_var##_data, vsf_test_usart_request_tx_irq_run, false) \
    }; \
    static vsf_test_usart_request_tx_irq_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "request-tx", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
#define VSF_TEST_USART_REQUEST_RX_IRQ_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_request_rx_irq_suite_t suite_var; \
    static vsf_test_usart_request_rx_irq_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_REQUEST_RX_IRQ_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_REQUEST_RX_IRQ_CASES(__##suite_var##_data, vsf_test_usart_request_rx_irq_run, true) \
    }; \
    static vsf_test_usart_request_rx_irq_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "request-rx", \
        .hw_req     = "uart1+la+host_send", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
#define VSF_TEST_USART_REQUEST_CANCEL_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_request_cancel_suite_t suite_var; \
    static vsf_test_usart_request_cancel_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_REQUEST_CANCEL_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_REQUEST_CANCEL_CASES(__##suite_var##_data, vsf_test_usart_request_cancel_run, false) \
    }; \
    static vsf_test_usart_request_cancel_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "cancel", \
        .hw_req     = "uart1+la", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
#define VSF_TEST_USART_RX_BULK_IRQ_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_bulk_irq_suite_t suite_var; \
    static vsf_test_usart_rx_bulk_irq_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_BULK_IRQ_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_BULK_IRQ_CASES(__##suite_var##_data, vsf_test_usart_rx_bulk_irq_run, true) \
    }; \
    static vsf_test_usart_rx_bulk_irq_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-bulk-irq", \
        .hw_req     = "uart1+host", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
#define VSF_TEST_USART_RX_FIFO_THRESHOLD_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_usart_rx_fifo_threshold_suite_t suite_var; \
    static vsf_test_usart_rx_fifo_threshold_case_t __##suite_var##_data[] = { \
        VSF_TEST_USART_RX_FIFO_THRESHOLD_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_USART_RX_FIFO_THRESHOLD_CASES(__##suite_var##_data, vsf_test_usart_rx_fifo_threshold_run, true) \
    }; \
    static vsf_test_usart_rx_fifo_threshold_suite_t suite_var = { \
        .usart      = VSF_BOARD_USART_INSTANCE, \
        .name       = name_str, \
        .purpose    = "rx-fifo-threshold", \
        .hw_req     = "uart1+host", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

/*============================ PROTOTYPES ====================================*/

/* ---- TX suites ---- */
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
void vsf_test_usart_baud_run(const vsf_test_usart_baud_case_t *c);
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
void vsf_test_usart_mode_run(const vsf_test_usart_mode_case_t *c);
#endif

/* ---- RX suites ---- */
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
void vsf_test_usart_rx_data_run(const vsf_test_usart_rx_data_case_t *c);
#endif

#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
void vsf_test_usart_rx_baud_run(const vsf_test_usart_rx_baud_case_t *c);
#endif

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
void vsf_test_usart_rx_mode_run(const vsf_test_usart_rx_mode_case_t *c);
#endif

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_irq_run(const vsf_test_usart_rx_irq_case_t *c);
#endif

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
void vsf_test_usart_rx_timeout_run(const vsf_test_usart_rx_timeout_case_t *c);
#endif

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_run(const vsf_test_usart_rx_parity_error_case_t *c);
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_run(const vsf_test_usart_rx_frame_error_case_t *c);
#endif

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_break_error_run(const vsf_test_usart_rx_break_error_case_t *c);
#endif

#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_overflow_error_run(const vsf_test_usart_rx_overflow_error_case_t *c);
#endif

#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
void vsf_test_usart_break_signal_run(const vsf_test_usart_break_signal_case_t *c);
#endif

#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
void vsf_test_usart_hw_flow_control_run(const vsf_test_usart_hw_flow_control_case_t *c);
#endif

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
void vsf_test_usart_tx_fifo_irq_run(const vsf_test_usart_tx_fifo_irq_case_t *c);
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_fifo_irq_run(const vsf_test_usart_rx_fifo_irq_case_t *c);
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
void vsf_test_usart_request_tx_irq_run(const vsf_test_usart_request_tx_irq_case_t *c);
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
void vsf_test_usart_request_rx_irq_run(const vsf_test_usart_request_rx_irq_case_t *c);
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
void vsf_test_usart_request_cancel_run(const vsf_test_usart_request_cancel_case_t *c);
#endif

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
void vsf_test_usart_rx_bulk_irq_run(const vsf_test_usart_rx_bulk_irq_case_t *c);
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
void vsf_test_usart_rx_fifo_threshold_run(const vsf_test_usart_rx_fifo_threshold_case_t *c);
#endif

// Framework types — included LAST so this header can be pulled into
// vsf_test.h without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_USART_H__ */
/* EOF */
