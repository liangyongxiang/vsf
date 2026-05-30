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

/*============================ MACROS ========================================*/

#define SG_TEST_BUF_SIZE        64

/*============================ LOCAL VARIABLES ===============================*/

static uint8_t __sg_src_buf[SG_TEST_BUF_SIZE * 8];
static uint8_t __sg_dst_buf[SG_TEST_BUF_SIZE * 8];
static volatile bool __sg_done;

/*============================ PROTOTYPES ====================================*/

static void __vsf_test_dma_sg_handler(void *target_ptr, vsf_dma_t *dma_ptr,
                                       int8_t channel, vsf_dma_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_test_dma_sg_handler(void *target_ptr, vsf_dma_t *dma_ptr,
                                       int8_t channel, vsf_dma_irq_mask_t irq_mask)
{
    (void)target_ptr;
    (void)dma_ptr;
    (void)channel;
    (void)irq_mask;
    __sg_done = true;
}

static void __sg_prepare_buffers(void)
{
    for (uint16_t i = 0; i < sizeof(__sg_src_buf); i++) {
        __sg_src_buf[i] = (uint8_t)(0xA5 + i);
    }
    memset(__sg_dst_buf, 0, sizeof(__sg_dst_buf));
}

static bool __sg_verify_two_segment(void)
{
    for (uint16_t i = 0; i < SG_TEST_BUF_SIZE; i++) {
        if (__sg_dst_buf[i] != __sg_src_buf[i]) {
            return false;
        }
    }
    for (uint16_t i = 0; i < SG_TEST_BUF_SIZE; i++) {
        if (__sg_dst_buf[SG_TEST_BUF_SIZE + i] != __sg_src_buf[SG_TEST_BUF_SIZE + i]) {
            return false;
        }
    }
    return true;
}

static bool __sg_verify_scatter_read(void)
{
    for (uint16_t seg = 0; seg < 4; seg++) {
        for (uint16_t i = 0; i < SG_TEST_BUF_SIZE; i++) {
            uint16_t src_idx = seg * SG_TEST_BUF_SIZE * 2 + i;
            uint16_t dst_idx = seg * SG_TEST_BUF_SIZE + i;
            if (__sg_dst_buf[dst_idx] != __sg_src_buf[src_idx]) {
                return false;
            }
        }
    }
    return true;
}

static bool __sg_verify_gather_write(void)
{
    for (uint16_t seg = 0; seg < 4; seg++) {
        for (uint16_t i = 0; i < SG_TEST_BUF_SIZE; i++) {
            uint16_t src_idx = seg * SG_TEST_BUF_SIZE + i;
            uint16_t dst_idx = seg * SG_TEST_BUF_SIZE * 2 + i;
            if (__sg_dst_buf[dst_idx] != __sg_src_buf[src_idx]) {
                return false;
            }
        }
    }
    return true;
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

    vsf_dma_channel_cfg_t ch_cfg = {
        .mode = VSF_DMA_MEMORY_TO_MEMORY
              | VSF_DMA_SRC_ADDR_INCREMENT
              | VSF_DMA_DST_ADDR_INCREMENT
              | VSF_DMA_SRC_WIDTH_BYTE_1
              | VSF_DMA_DST_WIDTH_BYTE_1,
        .irq_mask = VSF_DMA_IRQ_MASK_CPL,
    };
    err = vsf_dma_channel_config(dma, ch, &ch_cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    __sg_prepare_buffers();
    bool pass = true;

    switch (c->idx) {
    case 0: {
        /* --- Test 0: Two-segment M2M --- */
        vsf_trace_info("DMA:SG:TWO_SEGMENT_START" VSF_TRACE_CFG_LINEEND);

        vsf_dma_channel_sg_desc_t descs[2] = {
            {
                .mode = ch_cfg.mode,
                .src_address = (vsf_dma_addr_t)&__sg_src_buf[0],
                .dst_address = (vsf_dma_addr_t)&__sg_dst_buf[0],
                .count = SG_TEST_BUF_SIZE,
            },
            {
                .mode = ch_cfg.mode,
                .src_address = (vsf_dma_addr_t)&__sg_src_buf[SG_TEST_BUF_SIZE],
                .dst_address = (vsf_dma_addr_t)&__sg_dst_buf[SG_TEST_BUF_SIZE],
                .count = SG_TEST_BUF_SIZE,
            },
        };

        __sg_done = false;
        err = vsf_dma_channel_sg_config_desc(dma, ch,
            (vsf_dma_isr_t){ .handler_fn = __vsf_test_dma_sg_handler, .target_ptr = NULL },
            descs, dimof(descs));
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        err = vsf_dma_channel_sg_start(dma, ch);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        uint32_t timeout = 100000;
        while (!__sg_done && timeout--) {
            vsf_arch_sleep(0);
        }
        VSF_TEST_ASSERT(__sg_done);

        pass = __sg_verify_two_segment();
        vsf_trace_info("DMA:SG:TWO_SEGMENT_%s" VSF_TRACE_CFG_LINEEND,
                       pass ? "PASS" : "FAIL");
        break;
    }

    case 1: {
        /* --- Test 1: Scatter read --- */
        vsf_trace_info("DMA:SG:SCATTER_READ_START" VSF_TRACE_CFG_LINEEND);

        vsf_dma_channel_sg_desc_t descs[4];
        for (uint8_t seg = 0; seg < 4; seg++) {
            descs[seg].mode = ch_cfg.mode;
            descs[seg].src_address = (vsf_dma_addr_t)&__sg_src_buf[seg * SG_TEST_BUF_SIZE * 2];
            descs[seg].dst_address = (vsf_dma_addr_t)&__sg_dst_buf[seg * SG_TEST_BUF_SIZE];
            descs[seg].count = SG_TEST_BUF_SIZE;
        }

        __sg_done = false;
        err = vsf_dma_channel_sg_config_desc(dma, ch,
            (vsf_dma_isr_t){ .handler_fn = __vsf_test_dma_sg_handler, .target_ptr = NULL },
            descs, dimof(descs));
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        err = vsf_dma_channel_sg_start(dma, ch);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        uint32_t timeout = 100000;
        while (!__sg_done && timeout--) {
            vsf_arch_sleep(0);
        }
        VSF_TEST_ASSERT(__sg_done);

        pass = __sg_verify_scatter_read();
        vsf_trace_info("DMA:SG:SCATTER_READ_%s" VSF_TRACE_CFG_LINEEND,
                       pass ? "PASS" : "FAIL");
        break;
    }

    case 2: {
        /* --- Test 2: Gather write --- */
        vsf_trace_info("DMA:SG:GATHER_WRITE_START" VSF_TRACE_CFG_LINEEND);

        vsf_dma_channel_sg_desc_t descs[4];
        for (uint8_t seg = 0; seg < 4; seg++) {
            descs[seg].mode = ch_cfg.mode;
            descs[seg].src_address = (vsf_dma_addr_t)&__sg_src_buf[seg * SG_TEST_BUF_SIZE];
            descs[seg].dst_address = (vsf_dma_addr_t)&__sg_dst_buf[seg * SG_TEST_BUF_SIZE * 2];
            descs[seg].count = SG_TEST_BUF_SIZE;
        }

        __sg_done = false;
        err = vsf_dma_channel_sg_config_desc(dma, ch,
            (vsf_dma_isr_t){ .handler_fn = __vsf_test_dma_sg_handler, .target_ptr = NULL },
            descs, dimof(descs));
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        err = vsf_dma_channel_sg_start(dma, ch);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        uint32_t timeout = 100000;
        while (!__sg_done && timeout--) {
            vsf_arch_sleep(0);
        }
        VSF_TEST_ASSERT(__sg_done);

        pass = __sg_verify_gather_write();
        vsf_trace_info("DMA:SG:GATHER_WRITE_%s" VSF_TRACE_CFG_LINEEND,
                       pass ? "PASS" : "FAIL");
        break;
    }

    default:
        VSF_TEST_ASSERT(0);
        break;
    }

    VSF_TEST_ASSERT(pass);

    vsf_dma_channel_release(dma, ch);
    vsf_dma_fini(dma);
}

#endif /* VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED */
/* EOF */
