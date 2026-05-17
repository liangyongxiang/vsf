/*****************************************************************************
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

#include "vsf_test_flash_erase_program_read.h"

#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED

#include <string.h>

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS             200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_flash_erase_program_read_case_t __flash_erase_program_read_cases[] = {
    VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASES_INIT
};

static uint8_t __write_buf[256];
static uint8_t __read_buf[256];

/*============================ IMPLEMENTATION ================================*/

void vsf_test_flash_erase_program_read_add_cases(vsf_test_flash_erase_program_read_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_FLASH_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "flash_erase_program_read_%u offset=%lu size=%lu",
            (unsigned)__flash_erase_program_read_cases[i].idx,
            (unsigned long)__flash_erase_program_read_cases[i].offset,
            (unsigned long)__flash_erase_program_read_cases[i].size);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_flash_erase_program_read_run,
            __cfg_str_pool[i], (void *)&__flash_erase_program_read_cases[i]);
        __flash_erase_program_read_cases[i].scene = scene;
    }
}

void vsf_test_flash_erase_program_read_run(const vsf_test_flash_erase_program_read_case_t *c)
{
    vsf_flash_t *flash = c->scene->flash;
    uint32_t offset = c->offset;
    uint32_t size = c->size;

    VSF_TEST_ASSERT(size > 0);
    VSF_TEST_ASSERT(size <= sizeof(__write_buf));

    vsf_trace_info("FLASH:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    vsf_flash_capability_t cap = vsf_flash_capability(flash);
    VSF_TEST_ASSERT(cap.erase_sector_size > 0);
    VSF_TEST_ASSERT(cap.write_sector_size > 0);

    /* Align offset and size to sector boundaries. */
    uint32_t erase_offset = offset & ~(cap.erase_sector_size - 1);
    uint32_t erase_size = ((offset + size + cap.erase_sector_size - 1) & ~(cap.erase_sector_size - 1)) - erase_offset;

    /* Init flash. */
    vsf_err_t err = vsf_flash_init(flash, &(vsf_flash_cfg_t){
        .isr = {
            .handler_fn = NULL,
            .target_ptr = NULL,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_flash_enable(flash));

    /* Prepare test pattern. */
    for (uint32_t i = 0; i < size; i++) {
        __write_buf[i] = (uint8_t)(0xA5 + i);
    }

    /* Phase 1: Erase target region. */
    err = vsf_flash_erase_multi_sector(flash, erase_offset, erase_size);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Phase 2: Read back to verify erase (should be all 0xFF). */
    err = vsf_flash_read_multi_sector(flash, offset, __read_buf, size);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    for (uint32_t i = 0; i < size; i++) {
        VSF_TEST_ASSERT(__read_buf[i] == 0xFF);
    }

    /* Phase 3: Program test pattern. */
    err = vsf_flash_write_multi_sector(flash, offset, __write_buf, size);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Phase 4: Read back and verify. */
    err = vsf_flash_read_multi_sector(flash, offset, __read_buf, size);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    for (uint32_t i = 0; i < size; i++) {
        VSF_TEST_ASSERT(__read_buf[i] == __write_buf[i]);
    }

    vsf_trace_info("FLASH:ERASE_PROGRAM_READ:PASS offset=%lu size=%lu" VSF_TRACE_CFG_LINEEND,
                   (unsigned long)offset, (unsigned long)size);

    while (fsm_rt_cpl != vsf_flash_disable(flash));
    vsf_flash_fini(flash);
}

#endif /* VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED */

/* EOF */
