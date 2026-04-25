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

#ifndef __VSF_USR_CFG_QEMU_H__
#define __VSF_USR_CFG_QEMU_H__

/*============================ MACROS ========================================*/

#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                (16 * 1024)
#   define VSF_SYSTIMER_FREQ                            (25000000ul)

#define VSF_USE_TRACE                                   ENABLED
#define VSF_TRACE_CFG_COLOR_EN                          DISABLED

#define VSF_USE_LINUX                                   DISABLED
#define VSF_USE_POSIX                                   DISABLED

#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define USRAPP_CFG_STDIO_EN                             ENABLED
#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

#define VSF_ARCH_CFG_CALLSTACK_TRACE                    DISABLED
#define VSF_ASSERT(...)                                 if (!(__VA_ARGS__)) { while (1); }

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
