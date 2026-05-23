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

#ifndef __VSF_TEST_I2C_H__
#define __VSF_TEST_I2C_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#if     defined(__VSF_TEST_I2C_CLASS_IMPLEMENT)
#   undef __VSF_TEST_I2C_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_TEST_I2C_CASE_MAX_COUNT     16

#ifndef VSF_TEST_I2C_EEPROM_RW_ENABLE
#   define VSF_TEST_I2C_EEPROM_RW_ENABLE        DISABLED
#endif

#ifndef VSF_TEST_I2C_BUS_SCAN_ENABLE
#   define VSF_TEST_I2C_BUS_SCAN_ENABLE         DISABLED
#endif

#ifndef VSF_TEST_I2C_EEPROM_PAGE_ENABLE
#   define VSF_TEST_I2C_EEPROM_PAGE_ENABLE      ENABLED
#endif

/*============================ TYPES =========================================*/

// Per-suite context (populated by main.c)
vsf_class(vsf_test_i2c_eeprom_rw_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        /* Immutable suite config (set once by main.c, never modified by run). */
        vsf_i2c_t *i2c;
    )
    private_member(
        /* Per-case mutable state (run() MUST re-initialise before each case). */
        volatile vsf_i2c_irq_mask_t irq_mask;
    )
};

vsf_class(vsf_test_i2c_eeprom_page_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_i2c_t *i2c;
    )
    private_member(
        volatile vsf_i2c_irq_mask_t irq_mask;
    )
};

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
typedef struct vsf_test_i2c_eeprom_rw_case_t {
    uint8_t  idx;
    uint8_t  i2c_idx;
    uint8_t  eeprom_addr;
    uint8_t  mem_addr;
    uint8_t  data_len;
    vsf_test_i2c_eeprom_rw_suite_t *suite;
} vsf_test_i2c_eeprom_rw_case_t;
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
typedef struct vsf_test_i2c_eeprom_page_case_t {
    uint8_t  idx;
    uint8_t  i2c_idx;
    uint8_t  eeprom_addr;
    uint8_t  mem_addr;
    uint8_t  data_len;
    vsf_test_i2c_eeprom_page_suite_t *suite;
} vsf_test_i2c_eeprom_page_case_t;
#endif

/*============================ TYPES for bus_scan ============================*/

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
vsf_class(vsf_test_i2c_bus_scan_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_gpio_i2c_t      *gpio_i2c0;
        vsf_gpio_i2c_t      *gpio_i2c1;
    )
};

typedef struct vsf_test_i2c_bus_scan_case_t {
    uint8_t  scl_pin;
    uint8_t  sda_pin;
    vsf_gpio_i2c_t *gpio_i2c;
    vsf_test_i2c_bus_scan_suite_t *suite;
} vsf_test_i2c_bus_scan_case_t;
#endif

typedef struct vsf_test_i2c_suites_t {
    vsf_test_i2c_eeprom_rw_suite_t   eeprom_rw;
    vsf_test_i2c_bus_scan_suite_t    bus_scan;
    vsf_test_i2c_eeprom_page_suite_t eeprom_page;
} vsf_test_i2c_suites_t;

typedef struct vsf_test_i2c_cfg_t {
    vsf_i2c_t       *i2c;
    vsf_gpio_i2c_t  *gpio_i2c0;
    vsf_gpio_i2c_t  *gpio_i2c1;
    vsf_gpio_t      *gpio;          //!< for bus_scan gpio_i2c->port wiring
    bool (*setup)(vsf_test_suite_t *);
    void (*teardown)(vsf_test_suite_t *);
} vsf_test_i2c_cfg_t;

void vsf_test_i2c_init(vsf_test_i2c_suites_t *s, const vsf_test_i2c_cfg_t *cfg);

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
void vsf_test_i2c_eeprom_rw_add_cases(vsf_test_i2c_eeprom_rw_suite_t *suite);
void vsf_test_i2c_eeprom_rw_run(const vsf_test_i2c_eeprom_rw_case_t *c);
#endif

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
void vsf_test_i2c_bus_scan_add_cases(vsf_test_i2c_bus_scan_suite_t *suite);
void vsf_test_i2c_bus_scan_run(const vsf_test_i2c_bus_scan_case_t *c);
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
void vsf_test_i2c_eeprom_page_add_cases(vsf_test_i2c_eeprom_page_suite_t *suite);
void vsf_test_i2c_eeprom_page_run(const vsf_test_i2c_eeprom_page_case_t *c);
#endif

#include "test_params_generated.h"

// Framework types — included LAST so this header can be pulled into
// vsf_test.h (which needs vsf_test_i2c_suites_t) without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_I2C_H__ */
/* EOF */
