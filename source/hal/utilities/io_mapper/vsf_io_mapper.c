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

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_GPIO == ENABLED && VSF_HW_GPIO_COUNT > 0

/*============================ MACROS ========================================*/

#ifndef VSF_HW_GPIO_MASK
#   define VSF_HW_GPIO_MASK                 VSF_HAL_COUNT_TO_MASK(VSF_HW_GPIO_COUNT)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_IO_MAPPER_HW(__N, __BIT)      &VSF_MCONNECT(vsf_hw_gpio, __N),

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_io_mapper_type(vsf_hw) vsf_hw_io_mapper = {
    VSF_IO_MAPPER_INIT(VSF_HW_GPIO_COUNT, VSF_HW_IO_MAPPER_PORT_BITS_LOG2)

    .io = {
#define __VSF_HAL_TEMPLATE_MASK                             VSF_HW_GPIO_MASK
#define __VSF_HAL_TEMPLATE_MACRO                            __VSF_IO_MAPPER_HW
#define __VSF_HAL_TEMPLATE_ARG                              NULL
#include "hal/driver/common/template/vsf_template_instance_mask.h"
    }
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif      // VSF_HAL_USE_GPIO && VSF_HW_GPIO_COUNT > 0
