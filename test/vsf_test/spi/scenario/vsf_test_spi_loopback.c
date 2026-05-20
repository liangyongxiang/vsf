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

#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS            200
#endif

#define SPI_LOOPBACK_PATTERN_LEN               8

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_spi_loopback_case_t __spi_loopback_cases[] = {
    VSF_TEST_SPI_LOOPBACK_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_spi_loopback_add_cases(vsf_test_spi_loopback_scene_t *scene)
{
    scene->name    = "spi_loopback";
    scene->purpose = "spi_loopback";
    scene->hw_req  = "mosi-miso-jumper";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_SPI_LOOPBACK_CASE_COUNT; i++) {
        __spi_loopback_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_spi_loopback_run,
            (void *)&__spi_loopback_cases[i]);
    }
}

void vsf_test_spi_loopback_run(void *arg)
{
    vsf_test_spi_loopback_case_t *c = (vsf_test_spi_loopback_case_t *)arg;
    vsf_spi_t *spi = c->scene->spi;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    vsf_err_t err = vsf_spi_init(spi, &(vsf_spi_cfg_t){
        .mode      = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,
        .clock_hz  = 1000000,
        .isr       = { NULL, NULL, vsf_arch_prio_0 },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_spi_enable(spi));

    uint8_t tx_buf[SPI_LOOPBACK_PATTERN_LEN] = {
        0xA5, 0x5A, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC
    };
    uint8_t rx_buf[SPI_LOOPBACK_PATTERN_LEN] = {0};

    /* Activate CS (software) */
    vsf_spi_cs_active(spi, 0);

    uint_fast32_t tx_offset = 0, rx_offset = 0;
    vsf_spi_fifo_transfer(spi, tx_buf, &tx_offset,
                          rx_buf, &rx_offset,
                          SPI_LOOPBACK_PATTERN_LEN);

    vsf_spi_cs_inactive(spi, 0);

    /* With MOSI-MISO loopback jumper, rx should match tx */
    bool match = true;
    for (int i = 0; i < SPI_LOOPBACK_PATTERN_LEN; i++) {
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
