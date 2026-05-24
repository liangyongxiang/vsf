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

#include "vsf_test_dma_scatter_gather.h"

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_dma_scatter_gather_case_t __dma_scatter_gather_cases[] = {
    VSF_TEST_DMA_SCATTER_GATHER_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_dma_scatter_gather_add_cases(vsf_test_dma_scatter_gather_suite_t *suite)
{
    suite->name    = "dma_scatter_gather";
    suite->purpose = "dma_scatter_gather";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT; i++) {
        __dma_scatter_gather_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_dma_scatter_gather_run,
            (void *)&__dma_scatter_gather_cases[i]);
    }
}

void vsf_test_dma_scatter_gather_run(void *arg)
{
    vsf_test_dma_scatter_gather_case_t *c =
        (vsf_test_dma_scatter_gather_case_t *)arg;
    vsf_dma_t *dma = c->suite->dma;

    vsf_err_t err = vsf_dma_init(dma, &(vsf_dma_cfg_t){0});
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_dma_channel_hint_t hint = { .channel = -1 };
    err = vsf_dma_channel_acquire(dma, &hint);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    uint8_t ch = (uint8_t)hint.channel;

    /* RP2040 DMA does not support scatter-gather;
     * the API must return VSF_ERR_NOT_SUPPORT. */
    vsf_dma_channel_sg_desc_t desc = {0};
    err = vsf_dma_channel_sg_config_desc(dma, ch,
        (vsf_dma_isr_t){ NULL, NULL }, &desc, 1);
    VSF_TEST_ASSERT(err == VSF_ERR_NOT_SUPPORT);

    err = vsf_dma_channel_sg_start(dma, ch);
    VSF_TEST_ASSERT(err == VSF_ERR_NOT_SUPPORT);

    vsf_trace_info("DMA:SCATTER_GATHER:NOT_SUPPORT:PASS" VSF_TRACE_CFG_LINEEND);

    vsf_dma_channel_release(dma, ch);
    vsf_dma_fini(dma);
}

#endif /* VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED */
/* EOF */
