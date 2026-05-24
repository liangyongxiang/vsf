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

#ifndef __VSF_TEST_SPI_H__
#define __VSF_TEST_SPI_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_SPI_LOOPBACK_ENABLE
#   define VSF_TEST_SPI_LOOPBACK_ENABLE         ENABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_spi_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_spi_t *spi;
    )
};

vsf_class(vsf_test_spi_loopback_suite_t) {
    public_member(
        implement(vsf_test_spi_suite_base_t)
    )
};

typedef struct vsf_test_spi_suites_t {
    vsf_test_spi_loopback_suite_t loopback;
} vsf_test_spi_suites_t;


extern vsf_test_spi_suites_t vsf_test_spi_suites;
/*============================ PROTOTYPES ====================================*/

typedef struct vsf_test_spi_suite_binding_t {
    vsf_test_spi_suite_base_t *suite;
    vsf_spi_t               *instance;   //!< NULL = skip this suite
    bool (*setup)(vsf_test_suite_t *);
    void (*teardown)(vsf_test_suite_t *);
} vsf_test_spi_suite_binding_t;

void vsf_test_spi_init(vsf_test_spi_suites_t *s,
                         const vsf_test_spi_suite_binding_t bindings[],
                         uint8_t count);

#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
void vsf_test_spi_loopback_run(void *arg);
#endif

#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
void vsf_test_spi_loopback_add_cases(vsf_test_spi_loopback_suite_t *suite);
#endif
#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_SPI_H__ */
/* EOF */
