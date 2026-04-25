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

#ifndef __HAL_DRIVER_QEMU_FAKE_SOC_USART_H__
#define __HAL_DRIVER_QEMU_FAKE_SOC_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "hal/driver/common/template/vsf_template_hal_driver.h"

#if     defined(__VSF_HAL_HW_USART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_HW_USART_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
#include "../../__device.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS         VSF_USART_CFG_MULTI_CLASS
#endif

#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS   ENABLED

#define VSF_USART_CFG_DEC_PREFIX            vsf_hw
#define VSF_USART_CFG_DEC_UPCASE_PREFIX     VSF_HW

/*============================ TYPES =========================================*/

typedef enum vsf_usart_irq_mask_t {
    VSF_USART_IRQ_MASK_TX_CPL           = (0x1ul << 0),
    VSF_USART_IRQ_MASK_RX_CPL           = (0x1ul << 1),
    VSF_USART_IRQ_MASK_TX               = (0x1ul << 2),
    VSF_USART_IRQ_MASK_RX               = (0x1ul << 3),
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 4),
#   define VSF_USART_IRQ_MASK_RX_TIMEOUT VSF_USART_IRQ_MASK_RX_TIMEOUT
    VSF_USART_IRQ_MASK_CTS              = (0x1ul << 5),
#   define VSF_USART_IRQ_MASK_CTS       VSF_USART_IRQ_MASK_CTS
    VSF_USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 6),
#   define VSF_USART_IRQ_MASK_FRAME_ERR VSF_USART_IRQ_MASK_FRAME_ERR
    VSF_USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 7),
#   define VSF_USART_IRQ_MASK_BREAK_ERR VSF_USART_IRQ_MASK_BREAK_ERR
    VSF_USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 8),
#   define VSF_USART_IRQ_MASK_PARITY_ERR VSF_USART_IRQ_MASK_PARITY_ERR
    VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR  = (0x1ul << 9),
#   define VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR
    VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR  = (0x1ul << 10),
#   define VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR
    VSF_USART_IRQ_MASK_RX_IDLE          = (0x1ul << 12),
#   define VSF_USART_IRQ_MASK_RX_IDLE   VSF_USART_IRQ_MASK_RX_IDLE
} vsf_usart_irq_mask_t;

typedef struct vsf_usart_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
        struct {
            uint32_t is_busy             : 1;
            uint32_t rx_error_detected   : 1;
            uint32_t tx_error_detected   : 1;
            uint32_t rx_cancel           : 1;
            uint32_t tx_cancel           : 1;
            uint32_t reserved            : 27;
        };
    };
} vsf_usart_status_t;

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_usart.h"
#include "hal/driver/common/usart/usart_template.h"

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_HW_USART_CLASS_IMPLEMENT
#undef __VSF_HAL_HW_USART_CLASS_INHERIT__

#endif
#endif
/* EOF */
