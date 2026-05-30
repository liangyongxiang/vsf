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

#ifndef __VSF_TEST_DMA_SCATTER_GATHER_H__
#define __VSF_TEST_DMA_SCATTER_GATHER_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_dma.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT
#   define VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT     1
#endif

#ifndef VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE
#   define VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE        64
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
typedef struct {
    uint8_t sg_src_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 8];
    uint8_t sg_dst_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 8];
    volatile bool sg_done;
} vsf_test_dma_scatter_gather_var_t;
#endif
/*============================ PROTOTYPES ====================================*/

void vsf_test_dma_scatter_gather_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_DMA_SCATTER_GATHER_H__ */
/* EOF */
