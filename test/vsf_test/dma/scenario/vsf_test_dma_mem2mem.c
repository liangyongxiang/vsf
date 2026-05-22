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

#include "vsf_test_dma_mem2mem.h"

#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED

/*============================ MACROS ========================================*/


#define DMA_MEM2MEM_BUF_SIZE                   32

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_dma_mem2mem_case_t __dma_mem2mem_cases[] = {
    VSF_TEST_DMA_MEM2MEM_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_dma_mem2mem_add_cases(vsf_test_dma_mem2mem_suite_t *suite)
{
    suite->name    = "dma_mem2mem";
    suite->purpose = "dma_mem2mem";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_DMA_MEM2MEM_CASE_COUNT; i++) {
        __dma_mem2mem_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_dma_mem2mem_run,
            (void *)&__dma_mem2mem_cases[i]);
    }
}

void vsf_test_dma_mem2mem_run(void *arg)
{
    vsf_test_dma_mem2mem_case_t *c = (vsf_test_dma_mem2mem_case_t *)arg;
    vsf_dma_t *dma = c->suite->dma;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    uint8_t src_buf[DMA_MEM2MEM_BUF_SIZE];
    uint8_t dst_buf[DMA_MEM2MEM_BUF_SIZE] = {0};

    for (int i = 0; i < DMA_MEM2MEM_BUF_SIZE; i++) {
        src_buf[i] = (uint8_t)(0xA5 + i);
    }

    vsf_err_t err = vsf_dma_init(dma, &(vsf_dma_cfg_t){0});
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_dma_channel_hint_t hint = {
        .channel = -1,
    };
    err = vsf_dma_channel_acquire(dma, &hint);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    uint8_t ch = (uint8_t)hint.channel;

    err = vsf_dma_channel_config(dma, ch, &(vsf_dma_channel_cfg_t){
        .mode = VSF_DMA_MEMORY_TO_MEMORY
              | VSF_DMA_SRC_ADDR_INCREMENT
              | VSF_DMA_DST_ADDR_INCREMENT,
        .isr = { NULL, NULL },
        .prio = vsf_arch_prio_invalid,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_dma_channel_start(dma, ch,
                                (vsf_dma_addr_t)src_buf,
                                (vsf_dma_addr_t)dst_buf,
                                DMA_MEM2MEM_BUF_SIZE);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Poll until transfer completes */
    uint32_t timeout = 10000;
    while (vsf_dma_channel_status(dma, ch).is_busy && timeout-- > 0);
    VSF_TEST_ASSERT(timeout > 0);

    uint32_t transferred = vsf_dma_channel_get_transferred_count(dma, ch);
    VSF_TEST_ASSERT(transferred == DMA_MEM2MEM_BUF_SIZE);

    bool match = true;
    for (int i = 0; i < DMA_MEM2MEM_BUF_SIZE; i++) {
        if (dst_buf[i] != src_buf[i]) {
            match = false;
            break;
        }
    }
    VSF_TEST_ASSERT(match);

    if (match) {
        vsf_trace_info("DMA:MEM2MEM:PASS" VSF_TRACE_CFG_LINEEND);
    }

    vsf_dma_channel_release(dma, ch);
    vsf_dma_fini(dma);
}

#endif /* VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED */
/* EOF */
