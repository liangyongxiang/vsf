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

#include "vsf_test_dma_mem2mem_irq.h"

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#define DMA_MEM2MEM_IRQ_BUF_SIZE                   32

/*============================ LOCAL VARIABLES ===============================*/

static void __dma_mem2mem_irq_handler(void *target_ptr, vsf_dma_t *dma_ptr,
                                       int8_t channel, vsf_dma_irq_mask_t irq_mask)
{
    (void)dma_ptr;
    (void)channel;
    (void)irq_mask;
    vsf_test_dma_mem2mem_irq_suite_t *suite =
        (vsf_test_dma_mem2mem_irq_suite_t *)target_ptr;
    suite->irq_fired = true;
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_dma_mem2mem_irq_run(void *arg)
{
    vsf_test_dma_mem2mem_irq_case_t *c = (vsf_test_dma_mem2mem_irq_case_t *)arg;
    vsf_dma_t *dma = c->suite->dma;

    uint8_t src_buf[DMA_MEM2MEM_IRQ_BUF_SIZE];
    uint8_t dst_buf[DMA_MEM2MEM_IRQ_BUF_SIZE] = {0};

    for (int i = 0; i < DMA_MEM2MEM_IRQ_BUF_SIZE; i++) {
        src_buf[i] = (uint8_t)(0xA5 + i);
    }

    c->suite->irq_fired = false;

    vsf_err_t err = vsf_dma_init(dma, &(vsf_dma_cfg_t){0});
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_dma_channel_hint_t hint = { .channel = -1 };
    err = vsf_dma_channel_acquire(dma, &hint);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    uint8_t ch = (uint8_t)hint.channel;

    err = vsf_dma_channel_config(dma, ch, &(vsf_dma_channel_cfg_t){
        .mode = VSF_DMA_MEMORY_TO_MEMORY
              | VSF_DMA_SRC_ADDR_INCREMENT
              | VSF_DMA_DST_ADDR_INCREMENT,
        .isr = {
            .handler_fn = __dma_mem2mem_irq_handler,
            .target_ptr = c->suite,
        },
        .irq_mask = VSF_DMA_IRQ_MASK_CPL,
        .prio = vsf_arch_prio_invalid,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_dma_channel_start(dma, ch,
                                (vsf_dma_addr_t)src_buf,
                                (vsf_dma_addr_t)dst_buf,
                                DMA_MEM2MEM_IRQ_BUF_SIZE);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Wait for IRQ (with timeout fallback) */
    uint32_t timeout = 10000;
    while (!c->suite->irq_fired && timeout-- > 0);
    VSF_TEST_ASSERT(c->suite->irq_fired);

    uint32_t transferred = vsf_dma_channel_get_transferred_count(dma, ch);
    VSF_TEST_ASSERT(transferred == DMA_MEM2MEM_IRQ_BUF_SIZE);

    bool match = true;
    for (int i = 0; i < DMA_MEM2MEM_IRQ_BUF_SIZE; i++) {
        if (dst_buf[i] != src_buf[i]) {
            match = false;
            break;
        }
    }
    VSF_TEST_ASSERT(match);

    if (match) {
        vsf_trace_info("DMA:MEM2MEM_IRQ:PASS" VSF_TRACE_CFG_LINEEND);
    }

    vsf_dma_channel_release(dma, ch);
    vsf_dma_fini(dma);
}

#endif /* VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED */
/* EOF */
