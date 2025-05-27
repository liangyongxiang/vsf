/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
 ****************************************************************************/

#ifndef __VSF_TEMPLATE_ETH_H__
#define __VSF_TEMPLATE_ETH_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ ETHROS ========================================*/

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability.
 *
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_ETH_CFG_MULTI_CLASS
#   define VSF_ETH_CFG_MULTI_CLASS                      ENABLED
#endif

#if defined(VSF_HW_ETH_COUNT) && !defined(VSF_HW_ETH_MASK)
/**
 * \~english
 * @brief Define ETH hardware mask if count is defined
 *
 * \~chinese
 * @brief 如果定义了 ETH 硬件数量，则定义对应的掩码
 */
#   define VSF_HW_ETH_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_ETH_COUNT)
#endif

#if defined(VSF_HW_ETH_MASK) && !defined(VSF_HW_ETH_COUNT)
/**
 * \~english
 * @brief Define ETH hardware count if mask is defined
 *
 * \~chinese
 * @brief 如果定义了 ETH 硬件掩码，则定义对应的数量
 */
#   define VSF_HW_ETH_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_ETH_MASK)
#endif

/**
 * \~english
 * @brief We can redefine ethro VSF_ETH_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * Example:
 * @code
 * #define VSF_ETH_CFG_PREFIX    my_device    // Use my device hardware ETH driver
 * vsf_eth_init()                             // Will be expanded to my_device_eth_init()
 * @endcode
 * @note This ethro is only valid when VSF_ETH_CFG_FUNCTION_RENAME is ENABLED.
 *
 * \~chinese
 * @brief 可重新定义宏 VSF_ETH_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动。
 * 示例:
 * @code
 * #define VSF_ETH_CFG_PREFIX    my_device    // 使用自定义设备的 ETH 驱动
 * vsf_eth_init()                             // 将展开为 my_device_eth_init()
 * @endcode
 * @note 这个宏仅在 VSF_ETH_CFG_FUNCTION_RENAME 为 ENABLED 时有效。
 */
#ifndef VSF_ETH_CFG_PREFIX
#   if (VSF_ETH_CFG_MULTI_CLASS == DISABLED) && defined(VSF_HW_ETH_COUNT) && (VSF_HW_ETH_COUNT != 0)
#       define VSF_ETH_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_ETH_CFG_PREFIX                       vsf
#   endif
#endif

/**
 * \~english
 * @brief After define VSF_ETH_CFG_FUNCTION_RENAME to DISABLED, calling any
 * vsf_eth_xxx() function will not be renamed. It has higher priority than
 * VSF_ETH_CFG_PREFIX
 *
 * \~chinese
 * @brief 定义宏 VSF_ETH_CFG_FUNCTION_RENAME 的值为 DISABLED 后，调用任何
 * vsf_eth_xxx() 函数将不会被重命名。它的优先级将高于 VSF_ETH_CFG_PREFIX。
 */
#ifndef VSF_ETH_CFG_FUNCTION_RENAME
#   define VSF_ETH_CFG_FUNCTION_RENAME                  ENABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable ethro
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE to redefine enum @ref vsf_eth_mode_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可启用宏 VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE
 * 来重新定义枚举 @ref vsf_eth_mode_t。
 */
#ifndef VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable ethro
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG to redefine struct
 * @ref vsf_eth_cfg_t. For compatibility, members should not be
 * deleted when redefining it.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG
 * 来重新定义结构体 @ref vsf_eth_cfg_t。为保证兼容性，重新定义时不应
 * 删除成员。
 */
#if VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS to redefine struct vsf_eth_status_t.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS
 * 来重新定义结构体 vsf_eth_status_t。
 */
#ifndef VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS    DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable ethro
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY to redefine struct
 * @ref vsf_eth_capability_t. For compatibility, members should not
 * be deleted when redefining it.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY 来重新定义结构体
 * @ref vsf_eth_capability_t。为保证兼容性，重新定义时不应删除成员。
 */
#if VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable ethro
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE to redefine struct
 * @ref vsf_eth_buf_mode_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE
 * 来重新定义结构体 @ref vsf_eth_buf_mode_t。
 */
#if VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE == DISABLED
#    define VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE DISABLED
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF == DISABLED
#   define VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF DISABLED
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SEND_BUF == DISABLED
#   define VSF_ETH_CFG_REIMPLEMENT_TYPE_SEND_BUF DISABLED
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_RECV_BUF == DISABLED
#   define VSF_ETH_CFG_REIMPLEMENT_TYPE_RECV_BUF DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable ethro
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_BUF to redefine struct @ref vsf_eth_buf_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_BUF
 * 来重新定义结构体 @ref vsf_eth_sg_buf_t。
 */
#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_BUF == DISABLED
#    define VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_BUF DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable ethro
 * VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK to redefine struct @ref vsf_eth_irq_mask_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK
 * 来重新定义结构体 @ref vsf_eth_irq_mask_t。
 */
#if VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
#    define VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable ethro
 * VSF_ETH_CFG_INHERIT_HAL_CAPABILITY to inherit the structure
 * @ref vsf_peripheral_capability_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_ETH_CFG_INHERIT_HAL_CAPABILITY
 * 来继承结构体 @ref vsf_peripheral_capability_t。
 */
#ifndef VSF_ETH_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_ETH_CFG_INHERIT_HAL_CAPABILITY            ENABLED
#endif

/*============================ ETHROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief ETH API template, used to generate ETH type, specific prefix function declarations, etc.
 * @param[in] __prefix The prefix used for generating ETH functions.
 *
 * \~chinese
 * @brief ETH API 模板，用于生成 ETH 类型、特定前缀的函数声明等。
 * @param[in] __prefix 用于生成 ETH 函数的前缀。
 */
#define VSF_ETH_APIS(__prefix) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,            eth, init,               VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                 eth, fini,               VSF_MCONNECT(__prefix, _eth_t) *eth_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, fsm_rt_t,             eth, enable,             VSF_MCONNECT(__prefix, _eth_t) *eth_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, fsm_rt_t,             eth, disable,            VSF_MCONNECT(__prefix, _eth_t) *eth_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                 eth, irq_enable,         VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                 eth, irq_disable,        VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,            eth, send_request,       VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_send_buf_t *buf_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,            eth, recv_request,       VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_recv_buf_t *buf_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,            eth, sg_send_request,    VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_sg_send_buf_t *buf_ptr, uint32_t sg_count) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,            eth, sg_recv_request,    VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_sg_recv_buf_t *buf_ptr, uint32_t sg_count) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,            eth, ctrl,               VSF_MCONNECT(__prefix, _eth_t) *eth_ptr, vsf_eth_ctrl_t ctrl, void *param) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_eth_status_t,     eth, status,             VSF_MCONNECT(__prefix, _eth_t) *eth_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_eth_capability_t, eth, capability,         VSF_MCONNECT(__prefix, _eth_t) *eth_ptr) \

/*============================ TYPES =========================================*/

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF ETH modes that can be reimplemented in specific HAL
 * drivers.
 * \~chinese
 * @brief 预定义的 VSF ETH 模式，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * The following modes must be retained even if not supported by hardware.
 * If hardware supports more modes, we can implement them in the hardware driver:
 * - VSF_ETH_MODE_NO_EARLY_WAKEUP
 * - VSF_ETH_MODE_EARLY_WAKEUP
 * - VSF_ETH_MODE_RESET_NONE
 * - VSF_ETH_MODE_RESET_CPU
 * - VSF_ETH_MODE_RESET_SOC
 * \~chinese
 * 以下的模式即使硬件不支持也必须保留，如果硬件支持更多模式，我们可以在硬件驱动里实现它：
 * - VSF_ETH_MODE_NO_EARLY_WAKEUP
 * - VSF_ETH_MODE_EARLY_WAKEUP
 * - VSF_ETH_MODE_RESET_NONE
 * - VSF_ETH_MODE_RESET_CPU
 * - VSF_ETH_MODE_RESET_SOC
 *
 * \~english
 * In specific drivers, we can implement optional modes. Optional modes require
 * the driver to provide one or more enumeration options, and provide ethros
 * with the same name (users can determine whether to support the mode at
 * compile time). If these options are N to 1, the corresponding MASK option is
 * also required (users can select different modes at runtime).
 * \~chinese
 * 在特定驱动里，我们可以实现可选的模式。可选模式需要该驱动提供一个或多个枚举选项，
 * 并且提供和枚举选项同名的宏（用户可以在编译时判断是否支持该模式）。
 * 如果这些选项是 N 选 1 的关系，还需要提供对应的 MASK
 * 选项（用户可以在运行时选择不同的模式）
 */
typedef enum vsf_eth_mode_t {
    VSF_ETH_MODE_SPEED_10M        = 1 << 0, //!< \~english 10M speed mode \~chinese 10M 速度模式
    VSF_ETH_MODE_SPEED_100M       = 1 << 1, //!< \~english 100M speed mode \~chinese 100M 速度模式
    VSF_ETH_MODE_SPEED_1000M      = 1 << 2, //!< \~english 1000M speed mode \~chinese 1000M 速度模式

    VSF_ETH_MODE_DUPLEX_HALF      = 1 << 3, //!< \~english Half duplex mode \~chinese 半双工模式
    VSF_ETH_MODE_DUPLEX_FULL      = 1 << 4, //!< \~english Full duplex mode \~chinese 全双工模式

    // TODO: add more modes as needed
} vsf_eth_mode_t;
#endif

/**
 * \~english
 * @brief ETH mode completion, used to simplify the definition of ETH mode.
 * \~chinese
 * @brief ETH 模式补全，用来简化 ETH 模式的定义
 */
enum {
#ifndef VSF_ETH_MODE_SPEED_MASK
    VSF_ETH_MODE_SPEED_MASK         = VSF_ETH_MODE_SPEED_10M | VSF_ETH_MODE_SPEED_100M | VSF_ETH_MODE_SPEED_1000M,
#endif

#ifndef VSF_ETH_MODE_DUPLEX_MASK
    VSF_ETH_MODE_DUPLEX_MASK        = VSF_ETH_MODE_DUPLEX_HALF | VSF_ETH_MODE_DUPLEX_FULL,
#endif

#ifndef VSF_ETH_MODE_ALL_BITS_MASK
    VSF_ETH_MODE_ALL_BITS_MASK      = VSF_ETH_MODE_SPEED_MASK | VSF_ETH_MODE_DUPLEX_MASK,
#endif
};

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_eth_irq_mask_t {
    VSF_ETH_IRQ_MASK_RX_AVAILABLE    = (1 << 0), //!< \~english RX data available interrupt mask \~chinese 接收数据可用中断掩码
    VSF_ETH_IRQ_MASK_TX_COMPLETE     = (1 << 1), //!< \~english TX complete interrupt mask \~chinese 发送完成中断掩码
    VSF_ETH_IRQ_MASK_SG_RX_AVAILABLE = (1 << 2), //!< \~english Scatter-gather RX available interrupt mask \~chinese 分散聚集接收可用中断掩码
    VSF_ETH_IRQ_MASK_SG_TX_COMPLETE  = (1 << 3), //!< \~english Scatter-gather TX complete interrupt mask \~chinese 分散聚集发送完成中断掩码
    VSF_ETH_IRQ_MASK_ERROR           = (1 << 4), //!< \~english Error interrupt mask \~chinese 错误中断掩码
} vsf_eth_irq_mask_t;
#endif

enum {
#ifndef VSF_ETH_IRQ_MASK_ALL
    VSF_ETH_IRQ_MASK_ALL = VSF_ETH_IRQ_MASK_RX_AVAILABLE
                         | VSF_ETH_IRQ_MASK_TX_COMPLETE
                         | VSF_ETH_IRQ_MASK_SG_RX_AVAILABLE
                         | VSF_ETH_IRQ_MASK_SG_TX_COMPLETE
                         | VSF_ETH_IRQ_MASK_ERROR,
#endif
};


#if VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english @brief ETH forward declaration.
 * \~chinese @brief ETH 前置声明.
 */
typedef struct vsf_eth_t vsf_eth_t;

/**
 * \~english @brief ETH interrupt handler type declaration.
 * @param[in,out] target_ptr: user defined target pointer
 * @param[in,out] eth_ptr: @ref vsf_eth_t watchdog timer structure pointer
 * \~chinese @brief ETH 中断处理函数的类型声明.
 * @param[in,out] target_ptr: 用户自定义目标指针
 * @param[in,out] eth_ptr: @ref vsf_eth_t 看门狗定时器结构指针
 */
typedef void vsf_eth_isr_handler_t(void *target_ptr, vsf_eth_t *eth_ptr,
                                    vsf_eth_irq_mask_t irq_mask, void *param);

/**
 * \~english @brief ETH interrupt configuration structure.
 * \~chinese @brief ETH 中断配置结构体.
 */
typedef struct vsf_eth_isr_t {
    vsf_eth_isr_handler_t  *handler_fn; //!< \~english Interrupt handler function pointer \~chinese 中断处理函数指针
    void                   *target_ptr; //!< \~english User-defined target pointer \~chinese 用户自定义目标指针
    vsf_arch_prio_t        prio;        //!< \~english Interrupt priority \~chinese 中断优先级
} vsf_eth_isr_t;

/**
 * \~english
 * @brief ETH configuration structure.
 *
 * \~chinese
 * @brief ETH 配置结构体
 */
typedef struct vsf_eth_cfg_t {
    vsf_eth_mode_t  mode;   //!< \~english ETH mode, refer to @ref vsf_eth_mode_t \~chinese ETH 模式，参考 @ref vsf_eth_mode_t
    uint8_t mac_address[6]; //!< \~english MAC address, 6 bytes \~chinese MAC 地址，6 字节
    vsf_eth_isr_t   isr;    //!< \~english ETH interrupt configuration, refer to @ref vsf_eth_isr_t \~chinese ETH 中断配置，参考 @ref vsf_eth_isr_t
} vsf_eth_cfg_t;
#endif

/**
 * \~english
 * @brief ETH status information structure
 * Contains the current status of ETH operations and FIFO thresholds
 * \~chinese
 * @brief ETH 状态信息结构体
 * 包含 ETH 操作的当前状态和 FIFO 阈值
 */
#if VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_eth_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
        struct {
            uint32_t is_rx_available         : 1; //!< \~english RX data is available \~chinese 接收数据可用
            uint32_t is_tx_ready             : 1; //!< \~english TX is ready to accept data \~chinese 发送准备就绪，可接受数据
        };
    };
} vsf_eth_status_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief Predefined VSF ETH capability that can be reimplemented in specific hal drivers.
 * The vsf_eth_capability_t structure defines the features supported by the ETH hardware.
 * When reimplementing this structure, all existing members must be preserved.
 * \~chinese
 * @brief 预定义的 VSF ETH 能力，可以在具体的 HAL 驱动重新实现。
 * vsf_eth_capability_t 结构体定义了 ETH 硬件支持的特性，重新实现这
 * 个结构体的时候需要保留所有现有的成员。
 */
typedef struct vsf_eth_capability_t {
#if VSF_ETH_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)    //!< \~english Inherit the capability of peripheral \~chinese 继承外设的能力
#endif

    union {
        uint32_t value;
        struct {
            //! \~english Buffer can be released immediately after
            //! vsf_eth_send_request(), If 0, buffer can only be released after
            //! TX completion interrupt VSF_ETH_IRQ_MASK_TX_COMPLETE
            //! \~chinese
            //! 缓冲区可以在调用 vsf_eth_send_request() 后立即释放，如果为 0，则只能在 TX 完成中断 VSF_ETH_IRQ_MASK_TX_COMPLETE 后 释放缓冲区
            uint32_t is_send_buf_releasable_immediately : 1;

            //! \~english SG buffer can be released immediately after
            //! vsf_eth_sg_send_request(), If 0, SG buffer can only be released after
            //! TX completion interrupt VSF_ETH_IRQ_MASK_SG_TX_COMPLETE
            //! \~chinese
            //! 分散-聚集缓冲区可以在调用 vsf_eth_sg_send_request() 后立即释放，如果为 0，则只能在 TX 完成中断 VSF_ETH_IRQ_MASK_SG_TX_COMPLETE 后释放缓冲区
            uint32_t is_sg_send_buf_releasable_immediately : 1;
        };
    };
} vsf_eth_capability_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE == DISABLED
typedef enum vsf_eth_buf_mode_t {
    VSF_ETH_BUF_MODE_TX_CHECKSUM_OFFLOAD = 1 << 0, //!< \~english TX checksum offload mode \~chinese 发送校验和卸载模式
    VSF_ETH_BUF_MODE_RX_CHECKSUM_OFFLOAD = 1 << 1, //!< \~english RX checksum offload mode \~chinese 接收校验和卸载模式

    // TODO: Add more modes as needed
} vsf_eth_buf_mode_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_BUF == DISABLED
typedef struct vsf_eth_buf_t {
    vsf_eth_buf_mode_t mode;
    void               *payload;
    size_t             len;
} vsf_eth_buf_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_SEND_BUF == DISABLED
//! \~english Send buffer type, refer to @ref vsf_eth_send_buf_t
//! \~chinese 发送缓冲区类型，参考 @ref vsf_eth_send_buf_t
typedef vsf_eth_buf_t vsf_eth_send_buf_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_RECV_BUF == DISABLED
//! \~english Receive buffer type, refer to @ref vsf_eth_recv_buf_t
//! \~chinese 接收缓冲区类型，参考 @ref vsf_eth_recv_buf_t
typedef vsf_eth_buf_t vsf_eth_recv_buf_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_SEND_BUF == DISABLED
//! \~english Scatter-gather send buffer type, refer to @ref vsf_eth_sg_send_buf_t
//! \~chinese 分散聚集发送缓冲区类型，参考 @ref vsf_eth_sg_send_buf_t
typedef vsf_eth_buf_t vsf_eth_sg_send_buf_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_RECV_BUF == DISABLED
//! \~english Scatter-gather receive buffer type, refer to @ref vsf_eth_sg_recv_buf_t
//! \~chinese 分散聚集接收缓冲区类型，参考 @ref vsf_eth_sg_recv_buf_t
typedef vsf_eth_buf_t vsf_eth_sg_recv_buf_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_RECV_BUF == DISABLED
typedef enum vsf_eth_ctrl_t {
    VSF_ETH_CTRL_GET_MAC_ADDRESS, //!< \~english Get MAC address (uint8_t *) \~chinese 获取 MAC 地址 (uint8_t *)

    // TODO: Add more control commands as needed
} vsf_eth_ctrl_t;
#endif

/**
 * \~english @brief ETH operation function pointer type, used for ETH Multi Class support
 * \~chinese @brief ETH 操作函数指针类型，用于 ETH Multi Class 支持
 */
typedef struct vsf_eth_op_t {
/// @cond
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_ETH_APIS(vsf)
} vsf_eth_op_t;

#if VSF_ETH_CFG_MULTI_CLASS == ENABLED
/**
 * \~english @brief ETH instance structure, used for ETH Multi Class support, not needed in non Multi Class mode
 * \~chinese @brief ETH 实例结构体，用于 ETH Multi Class 支持，非 Multi Class 模式下不需要
 */
struct vsf_eth_t  {
    const vsf_eth_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[in] cfg_ptr: a pointer to structure @ref vsf_eth_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if ETH was initialized, or a negative error code
 *
 * @note It is not necessary to call vsf_eth_fini() for deinitialization.
 *       vsf_eth_init() should be called before any other ETH API except vsf_eth_capability().
 *
 * \~chinese
 * @brief 初始化一个 ETH 实例
 * @param[in] eth_ptr：指向结构体 @ref vsf_eth_t 的指针
 * @param[in] cfg_ptr：指向结构体 @ref vsf_eth_cfg_t 的指针
 * @return vsf_err_t：如果 ETH 初始化成功返回 VSF_ERR_NONE，失败返回负数
 *
 * @note 初始化失败后不需要调用 vsf_eth_fini() 进行反初始化。
 *       必须在调用除 vsf_eth_capability() 外的其他 ETH API 之前调用 vsf_eth_init()。
 */
extern vsf_err_t vsf_eth_init(vsf_eth_t *eth_ptr, vsf_eth_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Finalize a ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @return none
 *
 * \~chinese
 * @brief 终止一个 ETH 实例
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @return 无
 */
extern void vsf_eth_fini(vsf_eth_t *eth_ptr);

/**
 * \~english
 * @brief Enable a ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @return fsm_rt_t: FSM_RT_CPL if ETH was enabled, fsm_rt_on_going if ETH is still enabling
 *
 * \~chinese
 * @brief 启用一个 ETH 实例
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @return fsm_rt_t: 如果 ETH 实例已启用返回 FSM_RT_CPL，如果 ETH 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_eth_enable(vsf_eth_t *eth_ptr);

/**
 * \~english
 * @brief Disable a ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @return fsm_rt_t: FSM_RT_CPL if ETH was disabled, fsm_rt_on_going if ETH is still disabling
 *
 * \~chinese
 * @brief 禁用一个 ETH 实例
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @return fsm_rt_t: 如果 ETH 实例已禁用返回 FSM_RT_CPL，如果 ETH 实例正在禁用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_eth_disable(vsf_eth_t *eth_ptr);

/**
 * \~english
 * @brief Enable ETH instance interrupt.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[in] isr_mask: interrupt mask, refer to @ref vsf_eth_irq_mask_t
 * @note This function should be called after vsf_eth_init() and before vsf_eth_enable()
 *
 * \~chinese
 * @brief 启用 ETH 实例中断
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @param[in] isr_mask: 中断掩码，参考 @ref vsf_eth_irq_mask_t
 * @note 此函数应在调用 vsf_eth_init() 后调用
 */
extern void vsf_eth_isr_enable(vsf_eth_t *eth_ptr, vsf_eth_irq_mask_t isr_mask);

/**
 * \~english
 * @brief Disable ETH instance interrupt.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[in] isr_mask: interrupt mask, refer to @ref vsf_eth_irq_mask_t
 * @note This function should be called after vsf_eth_init() and before vsf_eth_enable()
 *
 * \~chinese
 * @brief 禁用 ETH 实例中断
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @param[in] isr_mask: 中断掩码，参考 @ref vsf_eth_irq_mask_t
 * @note 此函数应在调用 vsf_eth_init() 后调用
 */
extern void vsf_eth_isr_disable(vsf_eth_t *eth_ptr, vsf_eth_irq_mask_t isr_mask);

/**
 * \~english
 * @brief Get the status of ETH instance
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @return vsf_eth_status_t: return all status of current ETH
 *
 * \~chinese
 * @brief 获取 ETH 实例的状态
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @return vsf_eth_status_t: 返回当前 ETH 的所有状态
 */
extern vsf_eth_status_t vsf_eth_status(vsf_eth_t *eth_ptr);

/**
 * \~english
 * @brief Get the capability of ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @return vsf_eth_capability_t: all capability of current ETH @ref vsf_eth_capability_t
 *
 * \~chinese
 * @brief 获取 ETH 实例的能力
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @return vsf_eth_capability_t: 返回当前 ETH 的所有能力 @ref vsf_eth_capability_t
 */
extern vsf_eth_capability_t vsf_eth_capability(vsf_eth_t *eth_ptr);

/**
 * \~english
 * @brief Send a buffer over ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[in] buf_ptr: pointer to the buffer to be sent, refer to @ref vsf_eth_send_buf_t
 * @return vsf_err_t: VSF_ERR_NONE if buffer was sent successfully, or a negative error code
 * @note If the capability is_send_buf_releasable_immediately is false,
 *      the buffer should remain valid until the transmission complete interrupt is received
 *
 * \~chinese
 * @brief 通过 ETH 实例发送一个缓冲区
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @param[in] buf_ptr: 指向要发送的缓冲区的指针，参考 @ref vsf_eth_send_buf_t
 * @return vsf_err_t: 如果缓冲区成功发送返回 VSF_ERR_NONE，失败返回负数
 * @note 如果 capability 的 is_send_buf_releasable_immediately 为 false， 在接收到发送完整中断之前，缓冲区应该保持有效
 * @note 如果 buf_ptr 的 payload 为 NULL，len 不为零，驱动如果可以支持对应大小的 buf，就返回 VSF_ERR_NONE，否则返回 VSF_ERR_INVALID_PARAMETER
 */
extern vsf_err_t vsf_eth_send_request(vsf_eth_t *eth_ptr, vsf_eth_send_buf_t *buf_ptr);

/**
 * \~english
 * @brief Send a scatter-gather buffer over ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[in] sg_ptr: pointer to the scatter-gather buffer array to be sent
 * @param[in] sg_count: number of scatter-gather buffer elements in the array
 * @return vsf_err_t: VSF_ERR_NONE if buffer was sent successfully, or a negative error code
 * @note The buffers should remain valid until the transmission complete interrupt is received
 *
 * \~chinese
 * @brief 通过 ETH 实例发送一个分散聚集的缓冲区
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @param[in] sg_ptr: 指向要发送的分散聚集的缓冲区数组的指针
 * @param[in] sg_count: 分散聚集的缓冲区数组中的元素数量
 * @return vsf_err_t: 如果缓冲区成功发送返回 VSF_ERR_NONE，失败返回负数
 * @note 在接收到发送完成中断之前，缓冲区应该保持有效
 */
extern vsf_err_t vsf_eth_sg_send_request(vsf_eth_t *eth_ptr, vsf_eth_sg_send_buf_t *sg_ptr, uint32_t sg_count);


/**
 * \~english
 * @brief Receive a buffer from ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[out] buf_ptr: pointer to the buffer to store received data, refer to @ref vsf_eth_recv_buf_t
 * @return vsf_err_t: VSF_ERR_NONE if buffer was received successfully, or a negative error code
 * @note The buffer must be pre-allocated with sufficient size to hold received data
 *
 * \~chinese
 * @brief 从 ETH 实例接收一个缓冲区
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @param[out] buf_ptr: 指向用于存储接收数据的缓冲区的指针，参考 @ref vsf_eth_recv_buf_t
 * @return vsf_err_t: 如果缓冲区成功接收返回 VSF_ERR_NONE，失败返回负数
 * @note 必须预先分配具有足够大小的缓冲区以存储接收到的数据
 * @note 如果 buf_ptr 的 payload 为 NULL，驱动将修改 len 返回可以读取的数据的长度
 * @note 如果 status 的 is_rx_available 为 1，而且 buf_ptr 的 payload 不为 NULL，
 *      驱动将复制数据到 payload 里，数据的长度是 buf_ptr->len 和 实际接收的数据长度的最小值
 *      如果 status 的 is_rx_available 为 0，驱动将返回 VSF_ERR_NOT_READY
 */
extern vsf_err_t vsf_eth_recv_request(vsf_eth_t *eth_ptr, vsf_eth_recv_buf_t *buf_ptr);

/**
 * \~english
 * @brief Receive data using scatter-gather buffers from ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[out] sg_ptr: pointer to the scatter-gather buffer array to store received data
 * @param[in] sg_count: number of scatter-gather buffer elements in the array
 * @return vsf_err_t: VSF_ERR_NONE if data was received successfully, or a negative error code
 * @note The buffers must be pre-allocated with sufficient size to hold received data
 *
 * \~chinese
 * @brief 从 ETH 实例使用分散聚集缓冲区接收数据
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @param[out] sg_ptr: 指向用于存储接收数据的分散聚集缓冲区数组的指针
 * @param[in] sg_count: 分散聚集缓缓冲区数组中的元素数量
 * @return vsf_err_t: 如果数据成功接收返回 VSF_ERR_NONE，失败返回负数
 * @note 必须预先分配具有足够大小的缓冲区以存储接收到的数据
 */
extern vsf_err_t vsf_eth_sg_recv_request(vsf_eth_t *eth_ptr, vsf_eth_sg_recv_buf_t *sg_ptr, uint32_t sg_count);

/**
 * \~english
 * @brief Control ETH instance.
 * @param[in] eth_ptr: a pointer to structure @ref vsf_eth_t
 * @param[in] ctrl_ptr: pointer to the control command, refer to @ref vsf_eth_ctrl_t
 * @param[in] param: control command parameter, the specific type depends on the control command
 *                   For example, VSF_ETH_CTRL_SET_MAC_ADDRESS requires a uint8_t* type parameter
 * @return vsf_err_t: VSF_ERR_NONE if control command was executed successfully, or a negative error code
 *
 * \~chinese
 * @brief 控制 ETH 实例
 * @param[in] eth_ptr: 指向结构体 @ref vsf_eth_t 的指针
 * @param[in] ctrl_ptr: 指向控制命令的指针，参考 @ref vsf_eth_ctrl_t
 * @param[in] param: 控制命令的参数，具体类型取决于控制命令
 *                   例如，VSF_ETH_CTRL_SET_MAC_ADDRESS 需要 uint8_t* 类型的参数
 * @return vsf_err_t: 如果控制命令成功执行返回 VSF_ERR_NONE，失败返回负数
 */
extern vsf_err_t vsf_eth_ctrl(vsf_eth_t *eth_ptr, vsf_eth_ctrl_t ctrl, void *param);

/*============================ ETHROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_ETH_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_eth_t                  VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_t)
#   define vsf_eth_init(__ETH, ...)     VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_init)((__vsf_eth_t *)(__ETH), ##__VA_ARGS__)
#   define vsf_eth_fini(__ETH)          VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_fini)((__vsf_eth_t *)(__ETH))
#   define vsf_eth_enable(__ETH)        VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_enable)((__vsf_eth_t *)(__ETH))
#   define vsf_eth_disable(__ETH)       VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_disable)((__vsf_eth_t *)(__ETH))
#   define vsf_eth_status(__ETH)        VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_status)((__vsf_eth_t *)(__ETH))
#   define vsf_eth_capability(__ETH)    VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_capability)((__vsf_eth_t *)(__ETH))
#   define vsf_eth_send_request(__ETH, ...)     VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_send_request)((__vsf_eth_t *)(__ETH), ##__VA_ARGS__)
#   define vsf_eth_sg_send_request(__ETH, ...)  VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_sg_send_request)((__vsf_eth_t *)(__ETH), ##__VA_ARGS__)
#   define vsf_eth_recv_request(__ETH, ...)     VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_recv_request)((__vsf_eth_t *)(__ETH), ##__VA_ARGS__)
#   define vsf_eth_sg_recv_request(__ETH, ...)  VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_sg_recv_request)((__vsf_eth_t *)(__ETH), ##__VA_ARGS__)
#   define vsf_eth_ctrl(__ETH, ...)     VSF_MCONNECT(VSF_ETH_CFG_PREFIX, _eth_ctrl)((__vsf_eth_t *)(__ETH), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_ETH_H__*/
