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

#include "vsf_test_spi_loopback.h"
#include "vsf_test_suites.h"

#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#define VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN              256

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_spi_loopback_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_spi_loopback_params_t *p = tc->arg;
    vsf_spi_t *spi = (vsf_spi_t *)fixture;

    uint16_t data_len = p->data_len;
    if (data_len == 0 || data_len > VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN) {
        data_len = VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN;
    }

    vsf_err_t err = vsf_spi_init(spi, &(vsf_spi_cfg_t){
        .mode      = VSF_SPI_MASTER | p->mode | VSF_SPI_DATASIZE_8,
        .clock_hz  = p->clock_hz,
        .isr       = { NULL, NULL, vsf_arch_prio_0 },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_spi_enable(spi));

    uint8_t tx_buf[VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN];
    uint8_t rx_buf[VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN] = {0};

    for (uint16_t i = 0; i < data_len; i++) {
        tx_buf[i] = (uint8_t)(0xA5 + i);
    }

    /* Activate CS (software) */
    vsf_spi_cs_active(spi, 0);

    uint_fast32_t tx_offset = 0, rx_offset = 0;
    vsf_spi_fifo_transfer(spi, tx_buf, &tx_offset,
                          rx_buf, &rx_offset,
                          data_len);

    vsf_spi_cs_inactive(spi, 0);

    /* With MOSI-MISO loopback jumper, rx should match tx */
    bool match = true;
    for (uint16_t i = 0; i < data_len; i++) {
        if (rx_buf[i] != tx_buf[i]) {
            match = false;
            break;
        }
    }

    if (match) {
        vsf_trace_info("SPI:LOOPBACK:PASS" VSF_TRACE_CFG_LINEEND);
    } else {
        vsf_trace_info("SPI:LOOPBACK:FAIL (no loopback jumper?)" VSF_TRACE_CFG_LINEEND);
    }

    while (fsm_rt_cpl != vsf_spi_disable(spi));
    vsf_spi_fini(spi);
}

#endif /* VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED */
/* EOF */
