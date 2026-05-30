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
 *  See the License for the specific language governing permissions and       *
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

#include "test_params_generated.h"

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

#ifndef VSF_TEST_I2C_SLAVE_ENABLE
#   define VSF_TEST_I2C_SLAVE_ENABLE            ENABLED
#endif

#ifndef VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE
#   define VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE   ENABLED
#endif

#ifndef VSF_TEST_I2C_SLAVE_FIFO_ENABLE
#   define VSF_TEST_I2C_SLAVE_FIFO_ENABLE       ENABLED
#endif

/*============================ TYPES =========================================*/







#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
vsf_class(vsf_test_i2c_eeprom_rw_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  i2c_idx;
        uint8_t  eeprom_addr;
        uint8_t  mem_addr;
        uint8_t  data_len;
    )
};
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
vsf_class(vsf_test_i2c_eeprom_page_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  i2c_idx;
        uint8_t  eeprom_addr;
        uint8_t  mem_addr;
        uint8_t  data_len;
    )
};
#endif

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
vsf_class(vsf_test_i2c_slave_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  master_i2c_idx;
        uint8_t  slave_i2c_idx;
    )
};
#endif

#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
vsf_class(vsf_test_i2c_eeprom_rw_fifo_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  i2c_idx;
        uint8_t  eeprom_addr;
        uint8_t  mem_addr;
        uint8_t  data_len;
    )
};
#endif

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
vsf_class(vsf_test_i2c_slave_fifo_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  master_i2c_idx;
        uint8_t  slave_i2c_idx;
    )
};
#endif

/*============================ TYPES for bus_scan ============================*/

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED

vsf_class(vsf_test_i2c_bus_scan_params_t) {
    public_member(
        uint8_t  idx;
        uint8_t  scl_pin;
        uint8_t  sda_pin;
    )
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
void vsf_test_i2c_eeprom_rw_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
void vsf_test_i2c_bus_scan_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
void vsf_test_i2c_eeprom_page_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
void vsf_test_i2c_slave_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
void vsf_test_i2c_eeprom_rw_fifo_run(vsf_test_case_t *tc);
#endif

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
void vsf_test_i2c_slave_fifo_run(vsf_test_case_t *tc);
#endif

// Framework types — included LAST so this header can be pulled into
// vsf_test.h (which needs vsf_test_i2c_suites_t) without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_I2C_H__ */
/* EOF */
