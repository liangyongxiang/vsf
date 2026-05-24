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

#include "vsf_test_i2c.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
vsf_test_i2c_suites_t vsf_test_i2c_suites;

void vsf_test_i2c_init(vsf_test_i2c_suites_t *s,
                         const vsf_test_i2c_suite_binding_t bindings[],
                         uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        vsf_test_i2c_suite_base_t *suite = bindings[i].suite;
        vsf_i2c_t                 *inst  = bindings[i].instance;
        if (inst == NULL) { continue; }

        suite->i2c  = inst;
        suite->setup  = bindings[i].setup;
        suite->teardown = bindings[i].teardown;
    }
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    vsf_test_i2c_eeprom_rw_add_cases(&s->eeprom_rw);
#endif

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    vsf_test_i2c_bus_scan_add_cases(&s->bus_scan);
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    vsf_test_i2c_eeprom_page_add_cases(&s->eeprom_page);
#endif

}
