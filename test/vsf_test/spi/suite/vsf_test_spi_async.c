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

#include "vsf_test_spi_async.h"
/*============================ LOCAL VARIABLES ===============================*/

#define SPI_ASYNC_MAX_DATA_LEN              256

static uint8_t __spi_async_tx_buf[SPI_ASYNC_MAX_DATA_LEN];
static uint8_t __spi_async_rx_buf[SPI_ASYNC_MAX_DATA_LEN];



#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED

/*============================ TYPES =========================================*/

typedef struct {
    volatile bool           done;
    vsf_spi_irq_mask_t      irq_mask;
} vsf_test_spi_async_ctx_t;

/*============================ PROTOTYPES ====================================*/

static void __vsf_test_spi_async_handler(void *target_ptr, vsf_spi_t *spi_ptr,
                                          vsf_spi_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_test_spi_async_handler(void *target_ptr, vsf_spi_t *spi_ptr,
                                          vsf_spi_irq_mask_t irq_mask)
{
    (void)spi_ptr;
    vsf_test_spi_async_ctx_t *ctx = (vsf_test_spi_async_ctx_t *)target_ptr;
    ctx->done = true;
    ctx->irq_mask = irq_mask;
}

static void __spi_async_prepare_buffers(uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        __spi_async_tx_buf[i] = (uint8_t)(0xA5 + i);
        __spi_async_rx_buf[i] = 0;
    }
}

void vsf_test_spi_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_spi_async_params_t *p = tc->arg;
    vsf_spi_t *spi = (vsf_spi_t *)fixture;
    vsf_test_spi_async_ctx_t ctx = { .done = false, .irq_mask = 0 };

    uint16_t data_len = p->data_len;
    if (data_len == 0 || data_len > SPI_ASYNC_MAX_DATA_LEN) {
        data_len = SPI_ASYNC_MAX_DATA_LEN;
    }

    vsf_err_t err = vsf_spi_init(spi, &(vsf_spi_cfg_t){
        .mode      = VSF_SPI_MASTER | p->mode | VSF_SPI_DATASIZE_8,
        .clock_hz  = p->clock_hz,
        .isr       = {
            .handler_fn = __vsf_test_spi_async_handler,
            .target_ptr = &ctx,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_spi_enable(spi));

    /* Activate CS (software) */
    vsf_spi_cs_active(spi, 0);

    bool pass = true;

    switch (p->test_type) {
    case 0: {
        /* --- Test 0: Full-duplex async transfer --- */
        vsf_trace_info("SPI:ASYNC:FULL_DUPLEX_START" VSF_TRACE_CFG_LINEEND);
        __spi_async_prepare_buffers(data_len);
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, __spi_async_tx_buf,
                                       __spi_async_rx_buf, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        /* Wait for completion */
        uint32_t timeout = 100000;
        while (!ctx.done && timeout--) {
            vsf_arch_sleep(0);
        }
        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.irq_mask & (VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_RX_CPL));

        /* Verify data */
        for (uint16_t i = 0; i < data_len; i++) {
            if (__spi_async_rx_buf[i] != __spi_async_tx_buf[i]) {
                pass = false;
                break;
            }
        }
        vsf_trace_info("SPI:ASYNC:FULL_DUPLEX_%s" VSF_TRACE_CFG_LINEEND,
                       pass ? "PASS" : "FAIL");
        break;
    }

    case 1: {
        /* --- Test 1: TX-only async transfer --- */
        vsf_trace_info("SPI:ASYNC:TX_ONLY_START" VSF_TRACE_CFG_LINEEND);
        __spi_async_prepare_buffers(data_len);
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, __spi_async_tx_buf, NULL, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        uint32_t timeout = 100000;
        while (!ctx.done && timeout--) {
            vsf_arch_sleep(0);
        }
        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.irq_mask & VSF_SPI_IRQ_MASK_TX_CPL);
        vsf_trace_info("SPI:ASYNC:TX_ONLY_PASS" VSF_TRACE_CFG_LINEEND);
        break;
    }

    case 2: {
        /* --- Test 2: RX-only async transfer --- */
        vsf_trace_info("SPI:ASYNC:RX_ONLY_START" VSF_TRACE_CFG_LINEEND);
        for (uint16_t i = 0; i < data_len; i++) {
            __spi_async_rx_buf[i] = 0;
        }
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, NULL, __spi_async_rx_buf, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        uint32_t timeout = 100000;
        while (!ctx.done && timeout--) {
            vsf_arch_sleep(0);
        }
        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.irq_mask & VSF_SPI_IRQ_MASK_RX_CPL);
        /* With loopback, RX should contain the dummy 0x00 bytes read back */
        vsf_trace_info("SPI:ASYNC:RX_ONLY_PASS" VSF_TRACE_CFG_LINEEND);
        break;
    }

    case 3: {
        /* --- Test 3: Cancel during transfer --- */
        vsf_trace_info("SPI:ASYNC:CANCEL_START" VSF_TRACE_CFG_LINEEND);
        __spi_async_prepare_buffers(data_len);
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, __spi_async_tx_buf,
                                       __spi_async_rx_buf, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        /* Cancel immediately (before completion) */
        vsf_spi_cancel_transfer(spi);

        /* Verify transfer was cancelled — ctx should NOT have completed
         * via the normal callback path since we cancelled. */
        VSF_TEST_ASSERT(!ctx.done);

        /* Verify SPI is no longer busy */
        vsf_spi_status_t status = vsf_spi_status(spi);
        VSF_TEST_ASSERT(!status.is_busy);

        vsf_trace_info("SPI:ASYNC:CANCEL_PASS" VSF_TRACE_CFG_LINEEND);
        break;
    }

    default:
        VSF_TEST_ASSERT(0);
        break;
    }

    vsf_spi_cs_inactive(spi, 0);
    while (fsm_rt_cpl != vsf_spi_disable(spi));
    vsf_spi_fini(spi);
}

#endif /* VSF_TEST_SPI_ASYNC_ENABLE == ENABLED */
/* EOF */
