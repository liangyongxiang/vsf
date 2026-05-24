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

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
vsf_test_dma_suites_t vsf_test_dma_suites;

void vsf_test_dma_init(vsf_test_dma_suites_t *s,
                         const vsf_test_dma_suite_binding_t bindings[],
                         uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        vsf_test_dma_suite_base_t *suite = bindings[i].suite;
        vsf_dma_t                *inst  = bindings[i].instance;
        if (inst == NULL) { continue; }

        suite->dma  = inst;
        suite->setup  = bindings[i].setup;
        suite->teardown = bindings[i].teardown;
    }
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    vsf_test_dma_mem2mem_add_cases(&s->mem2mem);
#endif

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    vsf_test_dma_mem2mem_irq_add_cases(&s->mem2mem_irq);
#endif

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    vsf_test_dma_scatter_gather_add_cases(&s->scatter_gather);
#endif

}


/* EOF */
