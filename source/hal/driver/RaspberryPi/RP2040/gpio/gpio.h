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

#ifndef __HAL_DRIVER_RP2040_GPIO_H__
#define __HAL_DRIVER_RP2040_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/* The minimal RP2040 GPIO driver uses the default vsf_gpio_mode_t enum
 * (VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE == DISABLED). Mode-bit to register
 * translation lives in gpio.c.
 *
 * Hardware reference (RP2040 datasheet, IO_BANK0 / PADS_BANK0 / SIO):
 *   IO_BANK0 GPIOn_CTRL
 *     [4:0]   FUNCSEL   pin function (5 = SIO for digital GPIO)
 *     [9:8]   OUTOVER   output override
 *     [13:12] OEOVER    output-enable override
 *     [17:16] INOVER    input override
 *     [29:28] IRQOVER   IRQ override
 *   PADS_BANK0 GPIOn
 *     [7] OD   output disable
 *     [6] IE   input enable
 *     [3] PUE  pull-up enable
 *     [2] PDE  pull-down enable
 *   SIO
 *     gpio_in / gpio_out (+ atomic gpio_set / gpio_clr / gpio_togl)
 *     gpio_oe / gpio_oe_set / gpio_oe_clr
 *
 * Mode → register mapping in this driver:
 *   VSF_GPIO_INPUT             : FUNCSEL=SIO, PADS.IE=1 OD=0, SIO.OE cleared on set_input
 *   VSF_GPIO_OUTPUT_PUSH_PULL  : FUNCSEL=SIO, PADS.IE=0 OD=0, SIO.OE set on set_output
 *   VSF_GPIO_OUTPUT_OPEN_DRAIN : FUNCSEL=SIO, PADS.OD=0, driver toggles SIO.OE to drive low / float
 *   VSF_GPIO_AF                : FUNCSEL = cfg.alternate_function
 *   VSF_GPIO_ANALOG            : FUNCSEL=NULL(0x1f), PADS.IE=0
 *   VSF_GPIO_EXTI              : Same as INPUT (EXTI not implemented in minimal driver)
 *   VSF_GPIO_PULL_UP / DOWN    : PADS.PUE / PADS.PDE
 *
 * EXTI APIs return VSF_ERR_NOT_SUPPORT in the minimal driver.
 */

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif      /* VSF_HAL_USE_GPIO */
#endif      /* __HAL_DRIVER_RP2040_GPIO_H__ */
/* EOF */
