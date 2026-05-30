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

#define __VSF_TEST_FLASH_CLASS_IMPLEMENT
#include "vsf_test_flash_boundary.h"

#include <string.h>
#include "vsf_test_suites.h"
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED

/*============================ LOCAL VARIABLES ===============================*/


/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_flash_boundary_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_flash_boundary_params_t *p = tc->arg;
    vsf_flash_t *flash = (vsf_flash_t *)fixture;
    uint32_t offset = p->offset;
    uint32_t size = p->size;

    VSF_TEST_ASSERT(size > 0);

    vsf_flash_capability_t cap = vsf_flash_capability(flash);
    VSF_TEST_ASSERT(cap.erase_sector_size > 0);
    VSF_TEST_ASSERT(cap.write_sector_size > 0);

    /* Ensure the write crosses at least one page boundary. */
    uint32_t page_mask = cap.write_sector_size - 1;
    uint32_t start_page = offset & ~page_mask;
    uint32_t end_page   = (offset + size - 1) & ~page_mask;
    VSF_TEST_ASSERT(start_page != end_page);

    /* Align to sector for erase. */
    uint32_t erase_offset = offset & ~(cap.erase_sector_size - 1);
    uint32_t erase_end    = (offset + size + cap.erase_sector_size - 1)
                          & ~(cap.erase_sector_size - 1);
    uint32_t erase_size   = erase_end - erase_offset;
    if (erase_size == 0) {
        erase_size = cap.erase_sector_size;
    }

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
        vsf_test_suite_data.flash_boundary.write_buf[i] = (uint8_t)(0xA5 + i);
    }

    /* Phase 1: Erase. */
    err = vsf_flash_erase_multi_sector(flash, erase_offset, erase_size);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Phase 2: Program across page boundary. */
    err = vsf_flash_write_multi_sector(flash, offset, vsf_test_suite_data.flash_boundary.write_buf, size);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Phase 3: Read back and verify. */
    err = vsf_flash_read_multi_sector(flash, offset, vsf_test_suite_data.flash_boundary.read_buf, size);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    for (uint32_t i = 0; i < size; i++) {
        VSF_TEST_ASSERT(vsf_test_suite_data.flash_boundary.read_buf[i] == vsf_test_suite_data.flash_boundary.write_buf[i]);
    }

    vsf_trace_info("FLASH:BOUNDARY:PASS offset=%lu size=%lu pages=%lu-%lu"
                   VSF_TRACE_CFG_LINEEND,
                   (unsigned long)offset, (unsigned long)size,
                   (unsigned long)(start_page / cap.write_sector_size),
                   (unsigned long)(end_page / cap.write_sector_size));

    while (fsm_rt_cpl != vsf_flash_disable(flash));
    vsf_flash_fini(flash);
}

#endif /* VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED */

/* EOF */