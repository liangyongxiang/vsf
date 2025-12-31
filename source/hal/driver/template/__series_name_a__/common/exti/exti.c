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

#if VSF_HAL_USE_EXTI == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${EXTI_IP}_EXTI_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${EXTI_IP}_EXTI_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${EXTI_IP}_EXTI_CLASS_IMPLEMENT
// IPCore end
// HW using ${EXTI_IP} IPCore driver
#define __VSF_HAL_${EXTI_IP}_EXTI_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_EXTI_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${EXTI_IP}_EXTI_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_EXTI_CFG_MULTI_CLASS
#   define VSF_HW_EXTI_CFG_MULTI_CLASS           VSF_EXTI_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_EXTI_CFG_IMP_PREFIX                  vsf_hw
#define VSF_EXTI_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_EXTI_CFG_IMP_PREFIX                  vsf_${exti_ip}
#define VSF_EXTI_CFG_IMP_UPCASE_PREFIX           VSF_${EXTI_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) {
#if VSF_HW_EXTI_CFG_MULTI_CLASS == ENABLED
    vsf_exti_t                   vsf_exti;
#endif
    void                        *reg;
} VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t);
// HW end

/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw exti only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only.
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_init)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_fini)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
}

vsf_exti_status_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_status)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    return (vsf_exti_status_t) {
        .enable_mask = 0,
        .status_mask = 0,
    };
}

vsf_exti_capability_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_capability)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    return (vsf_exti_capability_t) {
        .support_level_trigger  = 1,
        .support_edge_trigger   = 1,
        .support_sw_trigger     = 1,
        .irq_num                = 1,
        .channel_mask           = (vsf_exti_channel_mask_t)-1,
    };
}

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_trigger)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    vsf_exti_channel_mask_t channel_mask
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    (void)channel_mask;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_config_channels)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    vsf_exti_channel_mask_t channel_mask,
    vsf_exti_channel_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    (void)channel_mask;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_enable)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    vsf_exti_channel_mask_t channel_mask
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    (void)channel_mask;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_irq_disable)(
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t) *exti_ptr,
    vsf_exti_channel_mask_t channel_mask
) {
    VSF_HAL_ASSERT(NULL != exti_ptr);
    (void)channel_mask;
    return VSF_ERR_NONE;
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw exti only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only.
 */

// HW
#define VSF_EXTI_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti_t)                               \
        VSF_MCONNECT(VSF_EXTI_CFG_IMP_PREFIX, _exti, __IDX) = {                  \
        .reg                = VSF_MCONNECT(VSF_EXTI_CFG_IMP_UPCASE_PREFIX,       \
                                           _EXTI, __IDX, _REG),                 \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/exti/exti_template.inc"
// HW end

#endif /* VSF_HAL_USE_EXTI */

