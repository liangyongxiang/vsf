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

#include "vsf_test_i2c_eeprom_rw.h"

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS             200
#endif
#ifndef VSF_TEST_I2C_CLOCK_HZ
#   define VSF_TEST_I2C_CLOCK_HZ                100000
#endif
#ifndef VSF_TEST_I2C_TIMEOUT_MS
#   define VSF_TEST_I2C_TIMEOUT_MS              1000
#endif
#ifndef VSF_TEST_I2C_EEPROM_WRITE_CYCLE_MS
#   define VSF_TEST_I2C_EEPROM_WRITE_CYCLE_MS   10
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_i2c_eeprom_rw_case_t __i2c_eeprom_rw_cases[] = {
    VSF_TEST_I2C_EEPROM_RW_CASES_INIT
};

static volatile vsf_i2c_irq_mask_t __irq_mask;

/*============================ LOCAL FUNCTIONS ===============================*/

static void __i2c_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                      vsf_i2c_irq_mask_t irq_mask)
{
    __irq_mask |= irq_mask;
}

static bool __i2c_wait_complete(uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (__irq_mask & VSF_I2C_IRQ_MASK_MASTER_ERR) {
            return false;
        }
        if (__irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            return true;
        }
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_i2c_eeprom_rw_add_cases(vsf_test_i2c_eeprom_rw_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_I2C_EEPROM_RW_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_I2C_CASE_MAX_COUNT][80];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "i2c_eeprom_rw_%u purpose=eeprom hw_req=i2c_eeprom addr=0x%02X len=%u",
            (unsigned)__i2c_eeprom_rw_cases[i].idx,
            (unsigned)__i2c_eeprom_rw_cases[i].eeprom_addr,
            (unsigned)__i2c_eeprom_rw_cases[i].data_len);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_i2c_eeprom_rw_run,
            __cfg_str_pool[i], (void *)&__i2c_eeprom_rw_cases[i]);
        __i2c_eeprom_rw_cases[i].scene = scene;
    }
}

void vsf_test_i2c_eeprom_rw_run(const vsf_test_i2c_eeprom_rw_case_t *c)
{
    vsf_i2c_t *i2c = c->scene->i2c;
    uint8_t data_len = c->data_len;
    static uint8_t write_buf[VSF_TEST_I2C_CASE_MAX_COUNT + 1];
    static uint8_t read_buf[VSF_TEST_I2C_CASE_MAX_COUNT];

    VSF_TEST_ASSERT(data_len > 0);
    VSF_TEST_ASSERT(data_len <= VSF_TEST_I2C_CASE_MAX_COUNT);

    vsf_trace_info("I2C:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    /* Init i2c master at standard 100kHz. */
    vsf_err_t err = vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS,
        .clock_hz   = VSF_TEST_I2C_CLOCK_HZ,
        .isr        = {
            .handler_fn = __i2c_isr,
            .target_ptr = NULL,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_i2c_enable(i2c));
    vsf_i2c_irq_enable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ERR | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    /* Build write buffer: [mem_addr, payload...]. */
    write_buf[0] = c->mem_addr;
    for (uint8_t i = 0; i < data_len; i++) {
        write_buf[1 + i] = (uint8_t)(0xA0 + i);
    }

    /* Phase 1: Write [mem_addr, payload] to EEPROM. */
    __irq_mask = 0;
    err = vsf_i2c_master_request(i2c, c->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        data_len + 1, write_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(VSF_TEST_I2C_TIMEOUT_MS));

    /* EEPROM internal write cycle. */
    vsf_test_busy_wait_ms(VSF_TEST_I2C_EEPROM_WRITE_CYCLE_MS);

    /* Phase 2a: Set memory address (write phase, no stop). */
    __irq_mask = 0;
    err = vsf_i2c_master_request(i2c, c->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_NO_STOP | VSF_I2C_CMD_7_BITS,
        1, &write_buf[0]);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(VSF_TEST_I2C_TIMEOUT_MS));

    /* Phase 2b: Restart, then read data_len bytes. */
    __irq_mask = 0;
    err = vsf_i2c_master_request(i2c, c->eeprom_addr,
        VSF_I2C_CMD_RESTART | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        data_len, read_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(VSF_TEST_I2C_TIMEOUT_MS));

    for (uint8_t i = 0; i < data_len; i++) {
        VSF_TEST_ASSERT(read_buf[i] == write_buf[1 + i]);
    }

    vsf_trace_info("I2C:EEPROM_RW:PASS len=%u" VSF_TRACE_CFG_LINEEND,
                   (unsigned)data_len);

    vsf_i2c_irq_disable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ERR | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c));
    vsf_i2c_fini(i2c);
}

#endif /* VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED */

/* EOF */
