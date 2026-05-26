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

/* The RP2040 GPIO driver reimplements vsf_gpio_mode_t so that mode bits
 * directly encode hardware register fields, eliminating manual translation
 * in gpio.c.
 *
 * Bit layout:
 *   [4:0]   FUNCSEL — written directly to IO_BANK0.GPIOx_CTRL
 *           5 = SIO (for INPUT/OUTPUT/EXTI), 0x1F = NULL (for ANALOG)
 *   [5]     is_output — direction hint; RP2040 direction is SIO.OE, not PADS
 *   [6]     OD_emulated — open-drain is software-emulated via OE toggling
 *   [7]     is_AF — alternate function mode, FUNCSEL from cfg.alternate_function
 *   [9:8]   pull: 0 = none, 1 = up, 2 = down
 *   [13:10] EXTI trigger — directly maps to IO_BANK0 INTR/INTE 4-bit field
 *           1 = LEVEL_LOW, 2 = LEVEL_HIGH, 4 = EDGE_LOW, 8 = EDGE_HIGH
 *
 * Hardware reference (RP2040 datasheet, IO_BANK0 / PADS_BANK0 / SIO):
 *   IO_BANK0 GPIOn_CTRL: [4:0] FUNCSEL
 *   PADS_BANK0 GPIOn:    [7] OD, [6] IE, [3] PUE, [2] PDE
 *   SIO:                 gpio_oe, gpio_out, gpio_in
 */

#define VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE      ENABLED

typedef enum vsf_gpio_mode_t {
    /* Base modes — FUNCSEL in bits [4:0], flags in bits [7:5] */
    VSF_GPIO_INPUT              = (5 << 0) | (0 << 5) | (0 << 6) | (0 << 7),
    VSF_GPIO_ANALOG             = (0x1F << 0) | (0 << 5) | (0 << 6) | (0 << 7),
    VSF_GPIO_OUTPUT_PUSH_PULL   = (5 << 0) | (1 << 5) | (0 << 6) | (0 << 7),
    VSF_GPIO_OUTPUT_OPEN_DRAIN  = (5 << 0) | (1 << 5) | (1 << 6) | (0 << 7),
    VSF_GPIO_AF                 = (0 << 0) | (0 << 5) | (0 << 6) | (1 << 7),
    VSF_GPIO_EXTI               = VSF_GPIO_INPUT,

    /* Pull-up / pull-down */
    VSF_GPIO_NO_PULL_UP_DOWN    = (0 << 8),
    VSF_GPIO_PULL_UP            = (1 << 8),
    VSF_GPIO_PULL_DOWN          = (2 << 8),

    /* EXTI trigger modes — values directly usable as RP2040 INTR/INTE field */
    VSF_GPIO_EXTI_MODE_NONE         = (0 << 10),
    VSF_GPIO_EXTI_MODE_LOW_LEVEL    = (1 << 10),
    VSF_GPIO_EXTI_MODE_HIGH_LEVEL   = (2 << 10),
    VSF_GPIO_EXTI_MODE_FALLING      = (4 << 10),
    VSF_GPIO_EXTI_MODE_RISING       = (8 << 10),
    VSF_GPIO_EXTI_MODE_RISING_FALLING = (4 << 10) | (8 << 10),
} vsf_gpio_mode_t;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif      /* VSF_HAL_USE_GPIO */
#endif      /* __HAL_DRIVER_RP2040_GPIO_H__ */
/* EOF */
