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

//! \note do not move this pre-processor statement to other places
#include "component/vsf_component_cfg.h"

#ifndef __VSF_MAL_CFG_H__
#define __VSF_MAL_CFG_H__

/*============================ MACROS ========================================*/

#ifndef VSF_MAL_ASSERT
#   define VSF_MAL_ASSERT               VSF_ASSERT
#endif

#ifndef VSF_MAL_USE_MIM_MAL
#   define VSF_MAL_USE_MIM_MAL          ENABLED
#endif

#ifndef VSF_MAL_USE_CACHED_MAL
#   define VSF_MAL_USE_CACHED_MAL       VSF_USE_HEAP
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */