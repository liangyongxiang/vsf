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

#ifndef __VSF_TEST_DMA_H__
#define __VSF_TEST_DMA_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_DMA_MEM2MEM_ENABLE
#   define VSF_TEST_DMA_MEM2MEM_ENABLE         ENABLED
#endif
#ifndef VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE
#   define VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE     ENABLED
#endif
#ifndef VSF_TEST_DMA_SCATTER_GATHER_ENABLE
#   define VSF_TEST_DMA_SCATTER_GATHER_ENABLE  ENABLED
#endif

/*============================ TYPES =========================================*/





#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
vsf_class(vsf_test_dma_mem2mem_params_t) {
    public_member(
        uint8_t  idx;
        bool     expect_pass;
    )
};
#endif

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
vsf_class(vsf_test_dma_mem2mem_irq_params_t) {
    public_member(
        uint8_t  idx;
        bool     expect_pass;
    )
};
#endif

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
vsf_class(vsf_test_dma_scatter_gather_params_t) {
    public_member(
        uint8_t  idx;
        bool     expect_pass;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
void vsf_test_dma_mem2mem_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
void vsf_test_dma_mem2mem_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
void vsf_test_dma_scatter_gather_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);
#endif

#ifdef __cplusplus
}
#endif


/*============================ SUITE TABLE ==================================*/

#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
#   define __vsf_test_dma_mem2mem_suite { .name = "dma_mem2mem", .cases = __dma_mem2mem_cases, .case_count = dimof(__dma_mem2mem_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_DMA },
#else
#   define __vsf_test_dma_mem2mem_suite
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
#   define __vsf_test_dma_mem2mem_irq_suite { .name = "dma_mem2mem_irq", .cases = __dma_mem2mem_irq_cases, .case_count = dimof(__dma_mem2mem_irq_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_DMA },
#else
#   define __vsf_test_dma_mem2mem_irq_suite
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
#   define __vsf_test_dma_scatter_gather_suite { .name = "dma_scatter_gather", .cases = __dma_scatter_gather_cases, .case_count = dimof(__dma_scatter_gather_cases), .peripheral_type = VSF_PERIPHERAL_TYPE_DMA },
#else
#   define __vsf_test_dma_scatter_gather_suite
#endif

#define VSF_TEST_DMA_SUITES \
    __vsf_test_dma_mem2mem_suite \
    __vsf_test_dma_mem2mem_irq_suite \
    __vsf_test_dma_scatter_gather_suite

#endif /* __VSF_TEST_DMA_H__ */
/* EOF */
