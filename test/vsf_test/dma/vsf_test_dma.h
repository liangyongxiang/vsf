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

vsf_class(vsf_test_dma_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_dma_t *dma;
    )
};

vsf_class(vsf_test_dma_mem2mem_suite_t) {
    public_member(
        implement(vsf_test_dma_suite_base_t)
    )
};

vsf_class(vsf_test_dma_mem2mem_irq_suite_t) {
    public_member(
        implement(vsf_test_dma_suite_base_t)
    )
    private_member(
        volatile bool irq_fired;
    )
};

vsf_class(vsf_test_dma_scatter_gather_suite_t) {
    public_member(
        implement(vsf_test_dma_suite_base_t)
    )
};

#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
typedef struct vsf_test_dma_mem2mem_case_t {
    uint8_t  idx;
    bool     expect_pass;
    vsf_test_dma_mem2mem_suite_t *suite;
} vsf_test_dma_mem2mem_case_t;
#endif

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
typedef struct vsf_test_dma_mem2mem_irq_case_t {
    uint8_t  idx;
    bool     expect_pass;
    vsf_test_dma_mem2mem_irq_suite_t *suite;
} vsf_test_dma_mem2mem_irq_case_t;
#endif

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
typedef struct vsf_test_dma_scatter_gather_case_t {
    uint8_t  idx;
    bool     expect_pass;
    vsf_test_dma_scatter_gather_suite_t *suite;
} vsf_test_dma_scatter_gather_case_t;
#endif

/*============================ STATIC INIT MACROS ============================*/

#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
#define VSF_TEST_DMA_MEM2MEM_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_dma_mem2mem_suite_t suite_var; \
    static vsf_test_dma_mem2mem_case_t __##suite_var##_data[] = { \
        VSF_TEST_DMA_MEM2MEM_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_DMA_MEM2MEM_CASES(__##suite_var##_data, vsf_test_dma_mem2mem_run, false) \
    }; \
    static vsf_test_dma_mem2mem_suite_t suite_var = { \
        .name       = name_str, \
        .purpose    = "dma_mem2mem", \
        .hw_req     = "none", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
#define VSF_TEST_DMA_MEM2MEM_IRQ_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_dma_mem2mem_irq_suite_t suite_var; \
    static vsf_test_dma_mem2mem_irq_case_t __##suite_var##_data[] = { \
        VSF_TEST_DMA_MEM2MEM_IRQ_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_DMA_MEM2MEM_IRQ_CASES(__##suite_var##_data, vsf_test_dma_mem2mem_irq_run, false) \
    }; \
    static vsf_test_dma_mem2mem_irq_suite_t suite_var = { \
        .name       = name_str, \
        .purpose    = "dma_mem2mem_irq", \
        .hw_req     = "none", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
#define VSF_TEST_DMA_SCATTER_GATHER_STATIC(suite_var, name_str, setup_fn, teardown_fn) \
    static vsf_test_dma_scatter_gather_suite_t suite_var; \
    static vsf_test_dma_scatter_gather_case_t __##suite_var##_data[] = { \
        VSF_TEST_DMA_SCATTER_GATHER_CASE_DATA(&suite_var) \
    }; \
    static vsf_test_case_t __##suite_var##_cases[] = { \
        VSF_TEST_DMA_SCATTER_GATHER_CASES(__##suite_var##_data, vsf_test_dma_scatter_gather_run, false) \
    }; \
    static vsf_test_dma_scatter_gather_suite_t suite_var = { \
        .name       = name_str, \
        .purpose    = "dma_scatter_gather", \
        .hw_req     = "none", \
        .setup      = setup_fn, \
        .teardown   = teardown_fn, \
        .cases      = __##suite_var##_cases, \
        .case_count = dimof(__##suite_var##_cases), \
    }
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
void vsf_test_dma_mem2mem_run(void *arg);
#endif

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
void vsf_test_dma_mem2mem_irq_run(void *arg);
#endif

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
void vsf_test_dma_scatter_gather_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_DMA_H__ */
/* EOF */
