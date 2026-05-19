/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_test_dma.h"

/*============================ IMPLEMENTATION ================================*/

#define REG_IF(gate, s, field, add_fn)            \
    do {                                          \
        vsf_test_shell_register_scene(vsf_test_get_shell(), gate); \
        add_fn(&(s)->field);                      \
    } while (0)

void vsf_test_dma_register_all(vsf_test_dma_scenes_t *s)
{
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    REG_IF("dma_mem2mem", s, mem2mem, vsf_test_dma_mem2mem_add_cases);
#endif
}

/* EOF */
