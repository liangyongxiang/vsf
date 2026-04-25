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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

#   define VSF_ARCH_PRI_NUM         8
#   define VSF_ARCH_PRI_BIT         3
#   define VSF_DEV_SWI_NUM          7

#else

#ifndef __HAL_DEVICE_QEMU_FAKE_SOC_DEVICE_H__
#define __HAL_DEVICE_QEMU_FAKE_SOC_DEVICE_H__

#define VSF_DEV_SWI_LIST            24,25,26,27,28,29,30

/*============================ INCLUDES ======================================*/

#include "CMSDK_CM7.h"
#include "../common/__common.h"

/*============================ MACROS ========================================*/

#define VSF_HW_USART_COUNT          1

#define VSF_HW_USART0_REG           CMSDK_UART0
#define VSF_HW_USART0_RX_IRQN       UART0RX_IRQn
#define VSF_HW_USART0_TX_IRQN       UART0TX_IRQn
#define VSF_HW_USART0_OVF_IRQN      UART_0_1_2_OVF_IRQn
#define VSF_HW_USART0_RX_IRQHandler UART0RX_Handler
#define VSF_HW_USART0_TX_IRQHandler UART0TX_Handler
#define VSF_HW_USART0_OVF_IRQHandler UART_0_1_2_OVF_Handler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
