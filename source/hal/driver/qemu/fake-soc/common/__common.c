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
#include "../__device.h"

/*============================ MACROS ========================================*/

#if !defined(__VSF_HAL_SWI_NUM)
#   define __VSF_DEV_SWI_NUM                VSF_DEV_SWI_NUM
#elif __VSF_HAL_SWI_NUM > VSF_ARCH_SWI_NUM
#   if (__VSF_HAL_SWI_NUM - VSF_ARCH_SWI_NUM) > VSF_DEV_SWI_NUM
#       define MFUNC_IN_U8_DEC_VALUE       (VSF_DEV_SWI_NUM)
#   else
#       define MFUNC_IN_U8_DEC_VALUE       (__VSF_HAL_SWI_NUM - VSF_ARCH_SWI_NUM)
#   endif
#   include "utilities/preprocessor/mf_u8_dec2str.h"
#   define __VSF_DEV_SWI_NUM               MFUNC_OUT_DEC_STR
#else
#   define __VSF_DEV_SWI_NUM               0
#endif

#define __QEMU_FAKE_SOC_SWI(__N, __VALUE)                                      \
    VSF_CAL_ROOT VSF_CAL_ISR(SWI##__N##_IRQHandler)                            \
    {                                                                          \
        if (__qemu_fake_soc_common.swi[__N].handler != NULL) {                 \
            __qemu_fake_soc_common.swi[__N].handler(                           \
                __qemu_fake_soc_common.swi[__N].pparam                         \
            );                                                                 \
        }                                                                      \
    }

#ifndef VSF_DEV_SWI_LIST
#   define VSF_DEV_SWI_LIST    0
#endif

/*============================ TYPES =========================================*/

#if VSF_DEV_SWI_NUM > 0
static const IRQn_Type __qemu_fake_soc_soft_irq[VSF_DEV_SWI_NUM] = {
    VSF_DEV_SWI_LIST
};
#endif

#if __VSF_DEV_SWI_NUM > 0
typedef struct __qemu_fake_soc_common_t {
    struct {
        vsf_swi_handler_t *handler;
        void *pparam;
    } swi[__VSF_DEV_SWI_NUM];
} __qemu_fake_soc_common_t;
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if __VSF_DEV_SWI_NUM > 0
static __qemu_fake_soc_common_t __qemu_fake_soc_common;
#endif

/*============================ IMPLEMENTATION ================================*/

#if __VSF_DEV_SWI_NUM > 0
VSF_MREPEAT(__VSF_DEV_SWI_NUM, __QEMU_FAKE_SOC_SWI, NULL)
#endif

static VSF_CAL_ALWAYS_INLINE vsf_err_t vsf_drv_swi_init(uint_fast8_t idx,
                                                vsf_arch_prio_t priority,
                                                vsf_swi_handler_t *handler,
                                                void *pparam)
{
#if __VSF_DEV_SWI_NUM > 0
    if (idx < __VSF_DEV_SWI_NUM) {
        if (handler != NULL) {
            __qemu_fake_soc_common.swi[idx].handler = handler;
            __qemu_fake_soc_common.swi[idx].pparam = pparam;

            NVIC_SetPriority(__qemu_fake_soc_soft_irq[idx], priority);
            NVIC_EnableIRQ(__qemu_fake_soc_soft_irq[idx]);
        } else {
            NVIC_DisableIRQ(__qemu_fake_soc_soft_irq[idx]);
        }
        return VSF_ERR_NONE;
    }
    VSF_HAL_ASSERT(false);
    return VSF_ERR_INVALID_RANGE;
#else
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
#endif
}

static VSF_CAL_ALWAYS_INLINE void vsf_drv_swi_trigger(uint_fast8_t idx)
{
#if __VSF_DEV_SWI_NUM > 0
    if (idx < __VSF_DEV_SWI_NUM) {
        NVIC_SetPendingIRQ(__qemu_fake_soc_soft_irq[idx]);
        return;
    }
#endif
    VSF_HAL_ASSERT(false);
}

#if __VSF_HAL_SWI_NUM > 0 || !defined(__VSF_HAL_SWI_NUM)
void vsf_drv_usr_swi_trigger(uint_fast8_t idx)
{
#if __VSF_HAL_SWI_NUM > VSF_ARCH_SWI_NUM || !defined(__VSF_HAL_SWI_NUM)
#   if defined(VSF_DEV_SWI_NUM) && VSF_DEV_SWI_NUM > 0
    if (idx < VSF_DEV_SWI_NUM) {
        vsf_drv_swi_trigger(idx);
        return;
    }
    idx -= VSF_DEV_SWI_NUM;
#   endif

#   if      (__VSF_HAL_SWI_NUM > VSF_ARCH_SWI_NUM + VSF_DEV_SWI_NUM)            \
        ||  !defined(__VSF_HAL_SWI_NUM)
#       if defined(VSF_USR_SWI_NUM) && VSF_USR_SWI_NUM > 0
    if (idx < VSF_USR_SWI_NUM) {
        vsf_usr_swi_trigger(idx);
        return;
    }
    idx -= VSF_USR_SWI_NUM;
#       endif
#   endif
#endif

    VSF_HAL_ASSERT(false);
}

vsf_err_t vsf_drv_usr_swi_init(uint_fast8_t idx,
                               vsf_arch_prio_t priority,
                               vsf_swi_handler_t *handler,
                               void *param)
{
#if __VSF_HAL_SWI_NUM > VSF_ARCH_SWI_NUM || !defined(__VSF_HAL_SWI_NUM)
#   if defined(VSF_DEV_SWI_NUM) && VSF_DEV_SWI_NUM > 0
    if (idx < VSF_DEV_SWI_NUM) {
        return vsf_drv_swi_init(idx, priority, handler, param);
    }
    idx -= VSF_DEV_SWI_NUM;
#   endif

#   if      (__VSF_HAL_SWI_NUM > VSF_ARCH_SWI_NUM + VSF_DEV_SWI_NUM)            \
        ||  !defined(__VSF_HAL_SWI_NUM)
#       if defined(VSF_USR_SWI_NUM) && VSF_USR_SWI_NUM > 0
    if (idx < VSF_USR_SWI_NUM) {
        return vsf_usr_swi_init(idx, priority, handler, param);
    }
    idx -= VSF_USR_SWI_NUM;
#       endif
#   endif
#endif

    VSF_HAL_ASSERT(false);
    return VSF_ERR_FAIL;
}
#endif
