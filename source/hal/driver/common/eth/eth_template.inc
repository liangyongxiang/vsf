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

#if VSF_HAL_USE_ETH == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ ETHROS ========================================*/

#define vsf_real_eth_t          VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t)
#define vsf_real_eth_capability VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_capability)

#ifndef VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifdef VSF_ETH_CFG_IMP_REMAP_PREFIX
#   undef VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY                ENABLED
#endif

#if VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_eth_capability_t vsf_real_eth_capability(vsf_real_eth_t *eth_ptr)
{
    vsf_eth_capability_t eth_capability = {
        0
    };

    return eth_capability;
}
#endif

/*============================ ETHROS ========================================*/

#undef VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY


#undef vsf_real_eth_t
#undef vsf_real_eth_capability

/*============================ ETHROS ========================================*/

#ifdef VSF_ETH_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_eth_t            VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t)
#   define vsf_imp_eth_init         VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_init)
#   define vsf_imp_eth_fini         VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_fini)
#   define vsf_imp_eth_enable       VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_enable)
#   define vsf_imp_eth_disable      VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_disable)
#   define vsf_imp_eth_capability   VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_capability)
#   define vsf_imp_eth_status       VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_status)

#   define vsf_remap_eth_t          VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_t)
#   define vsf_remap_eth_init       VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_init)
#   define vsf_remap_eth_fini       VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_fini)
#   define vsf_remap_eth_enable     VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_enable)
#   define vsf_remap_eth_disable    VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_disable)
#   define vsf_remap_eth_capability VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_capability)
#   define vsf_remap_eth_status     VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_status)

#   define VSF_ETH_CFG_IMP_REMAP_FUNCTIONS                                                        \
        vsf_err_t vsf_imp_eth_init(vsf_imp_eth_t *eth_ptr, vsf_eth_cfg_t *cfg_ptr)  \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_init(eth_ptr, cfg_ptr);                            \
        }                                                                           \
        void vsf_imp_eth_fini(vsf_imp_eth_t *eth_ptr)                               \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            vsf_remap_eth_fini(eth_ptr);                                            \
        }                                                                           \
        fsm_rt_t vsf_imp_eth_enable(vsf_imp_eth_t *eth_ptr)                         \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_enable(eth_ptr);                                   \
        }                                                                           \
        fsm_rt_t vsf_imp_eth_disable(vsf_imp_eth_t *eth_ptr)                        \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_disable(eth_ptr);                                  \
        }                                                                           \
        vsf_eth_status_t vsf_imp_eth_status(vsf_imp_eth_t *eth_ptr)                 \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_status(eth_ptr);                                   \
        }                                                                           \
        vsf_eth_capability_t vsf_imp_eth_capability(vsf_imp_eth_t *eth_ptr)         \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_capability(eth_ptr);                               \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _eth
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _ETH

#ifndef VSF_ETH_CFG_IMP_PREFIX
#   error "Please define VSF_ETH_CFG_IMP_PREFIX in eth driver"
#endif

#ifndef VSF_ETH_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_ETH_CFG_IMP_UPCASE_PREFIX in eth driver"
#endif

#ifndef VSF_ETH_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_ETH_CFG_IMP_COUNT_MASK_PREFIX        VSF_ETH_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_ETH_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_ETH_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_ETH_CFG_IMP_PREFIX
#undef VSF_ETH_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_ETH_CFG_IMP_UPCASE_PREFIX
#undef VSF_ETH_CFG_IMP_LV0
#undef VSF_ETH_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_ETH_CFG_IMP_HAS_OP
#undef VSF_ETH_CFG_IMP_EXTERN_OP

#undef vsf_imp_eth_t
#undef vsf_imp_eth_init
#undef vsf_imp_eth_fini
#undef vsf_imp_eth_enable
#undef vsf_imp_eth_disable
#undef vsf_imp_eth_status
#undef vsf_imp_eth_capability

#undef vsf_remap_eth_t
#undef vsf_remap_eth_init
#undef vsf_remap_eth_fini
#undef vsf_remap_eth_enable
#undef vsf_remap_eth_disable
#undef vsf_remap_eth_status
#undef vsf_remap_eth_capability

#endif  /* VSF_HAL_USE_ETH */
