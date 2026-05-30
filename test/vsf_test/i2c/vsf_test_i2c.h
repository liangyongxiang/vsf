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

vsf_class(vsf_test_i2c_suite_base_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_i2c_t *i2c;
    )
};

vsf_class(vsf_test_i2c_eeprom_rw_suite_t) {
    public_member(
        implement(vsf_test_i2c_suite_base_t)
    )
    private_member(
        volatile vsf_i2c_irq_mask_t irq_mask;
        uint8_t write_buf[VSF_TEST_I2C_CASE_MAX_COUNT + 1];
        uint8_t read_buf[VSF_TEST_I2C_CASE_MAX_COUNT];
    )
};

vsf_class(vsf_test_i2c_eeprom_page_suite_t) {
    public_member(
        implement(vsf_test_i2c_suite_base_t)
    )
    private_member(
        volatile vsf_i2c_irq_mask_t irq_mask;
        uint8_t write_buf[VSF_TEST_I2C_CASE_MAX_COUNT + 1];
        uint8_t read_buf[VSF_TEST_I2C_CASE_MAX_COUNT];
    )
};

vsf_class(vsf_test_i2c_slave_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_i2c_t *master_i2c;
        vsf_i2c_t *slave_i2c;
    )
};

vsf_class(vsf_test_i2c_eeprom_rw_fifo_suite_t) {
    public_member(
        implement(vsf_test_i2c_suite_base_t)
    )
    private_member(
        volatile vsf_i2c_irq_mask_t irq_mask;
        uint8_t            write_buf[VSF_TEST_I2C_CASE_MAX_COUNT + 1];
        uint8_t            read_buf[VSF_TEST_I2C_CASE_MAX_COUNT];
        volatile bool      done;
        volatile bool      error;
        vsf_i2c_cmd_t      cur_cmd;
        uint_fast16_t      offset;
    )
};

vsf_class(vsf_test_i2c_slave_fifo_suite_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_i2c_t *master_i2c;
        vsf_i2c_t *slave_i2c;
    )
    private_member(
        volatile vsf_i2c_irq_mask_t master_irq_mask;
        volatile vsf_i2c_irq_mask_t slave_irq_mask;
        uint8_t    master_buf[16];
        uint8_t    slave_buf[16];
        volatile uint_fast16_t  slave_rx_offset;
        volatile uint_fast16_t  slave_tx_offset;
        volatile bool  master_done;
        volatile bool  slave_complete;
    )
};

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
vsf_class(vsf_test_i2c_eeprom_rw_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  i2c_idx;
        uint8_t  eeprom_addr;
        uint8_t  mem_addr;
        uint8_t  data_len;
        vsf_test_i2c_eeprom_rw_suite_t *suite;
    )
};
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
vsf_class(vsf_test_i2c_eeprom_page_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  i2c_idx;
        uint8_t  eeprom_addr;
        uint8_t  mem_addr;
        uint8_t  data_len;
        vsf_test_i2c_eeprom_page_suite_t *suite;
    )
};
#endif

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
vsf_class(vsf_test_i2c_slave_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  master_i2c_idx;
        uint8_t  slave_i2c_idx;
        vsf_test_i2c_slave_suite_t *suite;
    )
};
#endif

#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
vsf_class(vsf_test_i2c_eeprom_rw_fifo_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  i2c_idx;
        uint8_t  eeprom_addr;
        uint8_t  mem_addr;
        uint8_t  data_len;
        vsf_test_i2c_eeprom_rw_fifo_suite_t *suite;
    )
};
#endif

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
vsf_class(vsf_test_i2c_slave_fifo_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  master_i2c_idx;
        uint8_t  slave_i2c_idx;
        vsf_test_i2c_slave_fifo_suite_t *suite;
    )
};
#endif

/*============================ TYPES for bus_scan ============================*/

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
vsf_class(vsf_test_i2c_bus_scan_suite_t) {
    public_member(
        implement(vsf_test_i2c_suite_base_t)
        vsf_gpio_i2c_t *gpio_i2c[2];
    )
    private_member(
        volatile vsf_i2c_irq_mask_t irq_mask;
    )
};

vsf_class(vsf_test_i2c_bus_scan_case_t) {
    public_member(
        uint8_t  idx;
        uint8_t  scl_pin;
        uint8_t  sda_pin;
        vsf_test_i2c_bus_scan_suite_t *suite;
    )
};
#endif

/*============================ STATIC TABLE TYPES ============================*/

VSF_TEST_DECLARE_TABLE(vsf_test_i2c_eeprom_rw_table_t, vsf_test_i2c_eeprom_rw_suite_t, vsf_test_i2c_eeprom_rw_case_t, VSF_TEST_I2C_EEPROM_RW_CASE_COUNT);
VSF_TEST_DECLARE_TABLE(vsf_test_i2c_bus_scan_table_t, vsf_test_i2c_bus_scan_suite_t, vsf_test_i2c_bus_scan_case_t, VSF_TEST_I2C_BUS_SCAN_CASE_COUNT);
VSF_TEST_DECLARE_TABLE(vsf_test_i2c_eeprom_page_table_t, vsf_test_i2c_eeprom_page_suite_t, vsf_test_i2c_eeprom_page_case_t, VSF_TEST_I2C_EEPROM_PAGE_CASE_COUNT);
VSF_TEST_DECLARE_TABLE(vsf_test_i2c_eeprom_rw_fifo_table_t, vsf_test_i2c_eeprom_rw_fifo_suite_t, vsf_test_i2c_eeprom_rw_fifo_case_t, VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_COUNT);
VSF_TEST_DECLARE_TABLE(vsf_test_i2c_slave_table_t, vsf_test_i2c_slave_suite_t, vsf_test_i2c_slave_case_t, VSF_TEST_I2C_SLAVE_CASE_COUNT);

/*============================ STATIC INIT MACROS ============================*/

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
#define VSF_TEST_I2C_EEPROM_RW_STATIC(suite_var, name_str) \
    static vsf_test_i2c_eeprom_rw_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_I2C_EEPROM_RW_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C, \
        }, \
        .data  = { VSF_TEST_I2C_EEPROM_RW_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_I2C_EEPROM_RW_CASES(suite_var.data, vsf_test_i2c_eeprom_rw_run, false) }, \
    }
#endif

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
#define VSF_TEST_I2C_BUS_SCAN_STATIC(suite_var, name_str) \
    static vsf_test_i2c_bus_scan_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_I2C_BUS_SCAN_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C, \
        }, \
        .data  = { VSF_TEST_I2C_BUS_SCAN_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_I2C_BUS_SCAN_CASES(suite_var.data, vsf_test_i2c_bus_scan_run, false) }, \
    }
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
#define VSF_TEST_I2C_EEPROM_PAGE_STATIC(suite_var, name_str) \
    static vsf_test_i2c_eeprom_page_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_I2C_EEPROM_PAGE_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C, \
        }, \
        .data  = { VSF_TEST_I2C_EEPROM_PAGE_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_I2C_EEPROM_PAGE_CASES(suite_var.data, vsf_test_i2c_eeprom_page_run, false) }, \
    }
#endif

#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
#define VSF_TEST_I2C_EEPROM_RW_FIFO_STATIC(suite_var, name_str) \
    static vsf_test_i2c_eeprom_rw_fifo_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C, \
        }, \
        .data  = { VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_I2C_EEPROM_RW_FIFO_CASES(suite_var.data, vsf_test_i2c_eeprom_rw_fifo_run, false) }, \
    }
#endif

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
#define VSF_TEST_I2C_SLAVE_STATIC(suite_var, name_str, master_ptr, slave_ptr) \
    static vsf_test_i2c_slave_table_t suite_var = { \
        .suite = { \
            .name       = name_str, \
            .cases      = suite_var.cases, \
            .case_count = VSF_TEST_I2C_SLAVE_CASE_COUNT, \
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C_SLAVE, \
            .master_i2c = master_ptr, \
            .slave_i2c  = slave_ptr, \
        }, \
        .data  = { VSF_TEST_I2C_SLAVE_CASE_DATA(&suite_var.suite) }, \
        .cases = { VSF_TEST_I2C_SLAVE_CASES(suite_var.data, vsf_test_i2c_slave_run, false) }, \
    }
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
void vsf_test_i2c_eeprom_rw_run(const vsf_test_i2c_eeprom_rw_case_t *c);
#endif

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
void vsf_test_i2c_bus_scan_run(const vsf_test_i2c_bus_scan_case_t *c);
#endif

#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
void vsf_test_i2c_eeprom_page_run(const vsf_test_i2c_eeprom_page_case_t *c);
#endif

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
void vsf_test_i2c_slave_run(void *arg);
#endif

#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
void vsf_test_i2c_eeprom_rw_fifo_run(const vsf_test_i2c_eeprom_rw_fifo_case_t *c);
#endif

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
void vsf_test_i2c_slave_fifo_run(void *arg);
#endif

// Framework types — included LAST so this header can be pulled into
// vsf_test.h (which needs vsf_test_i2c_suites_t) without circular issues.
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_I2C_H__ */
/* EOF */
