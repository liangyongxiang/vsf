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

#ifndef __VSF_TEST_SPI_LOOPBACK_H__
#define __VSF_TEST_SPI_LOOPBACK_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_spi.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_SPI_LOOPBACK_CASE_COUNT
#   define VSF_TEST_SPI_LOOPBACK_CASE_COUNT     1
#endif

#ifndef VSF_TEST_SPI_LOOPBACK_CASES_INIT
#define VSF_TEST_SPI_LOOPBACK_CASES_INIT                                       \
    { 0 }
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_spi_loopback_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_SPI_LOOPBACK_H__ */
/* EOF */
