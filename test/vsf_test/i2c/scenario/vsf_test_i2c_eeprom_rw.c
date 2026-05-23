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

#define __VSF_TEST_I2C_CLASS_IMPLEMENT
#include "vsf_test_i2c_eeprom_rw.h"

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_CLOCK_HZ
#   define VSF_TEST_I2C_CLOCK_HZ                100000
#endif
#ifndef VSF_TEST_I2C_TIMEOUT_MS
#   define VSF_TEST_I2C_TIMEOUT_MS              1000
#endif
#ifndef VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS
#   define VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS  100
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_i2c_eeprom_rw_case_t __i2c_eeprom_rw_cases[] = {
    VSF_TEST_I2C_EEPROM_RW_CASES_INIT
};

/*============================ LOCAL FUNCTIONS ===============================*/

static void __i2c_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                      vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    vsf_test_i2c_eeprom_rw_suite_t *suite = (vsf_test_i2c_eeprom_rw_suite_t *)target_ptr;
    suite->irq_mask |= irq_mask;
}

typedef enum {
    I2C_POLL_COMPLETE,
    I2C_POLL_ERR,
    I2C_POLL_TIMEOUT,
} __i2c_poll_result_t;

static __i2c_poll_result_t __i2c_wait_result(vsf_test_i2c_eeprom_rw_suite_t *suite,
                                              uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (suite->irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            return I2C_POLL_COMPLETE;
        }
        if (suite->irq_mask & VSF_I2C_IRQ_MASK_MASTER_ERR) {
            return I2C_POLL_ERR;
        }
        vsf_test_busy_wait_ms(1);
    }
    return I2C_POLL_TIMEOUT;
}

static bool __i2c_wait_complete(vsf_test_i2c_eeprom_rw_suite_t *suite, uint32_t timeout_ms)
{
    return __i2c_wait_result(suite, timeout_ms) == I2C_POLL_COMPLETE;
}

/* ACK polling: send a 1-byte write and check for ACK vs NAK.
 * EEPROM NAKs while its internal write cycle is in progress.
 * A single-byte write only updates the internal address pointer
 * and does not trigger another write cycle (no data bytes follow). */
static bool __eeprom_ack_poll(vsf_test_i2c_eeprom_rw_suite_t *suite,
                               vsf_i2c_t *i2c, uint8_t eeprom_addr,
                               uint8_t *dummy_buf, uint32_t max_ms)
{
    while (max_ms-- > 0) {
        suite->irq_mask = 0;
        vsf_err_t err = vsf_i2c_master_request(i2c, eeprom_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
            1, dummy_buf);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        __i2c_poll_result_t result = __i2c_wait_result(suite, 10);
        if (result == I2C_POLL_COMPLETE) {
            return true;
        }
        if (result == I2C_POLL_TIMEOUT) {
            return false;
        }
        /* I2C_POLL_ERR = NAK, EEPROM still busy – retry after 1 ms. */
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_i2c_eeprom_rw_add_cases(vsf_test_i2c_eeprom_rw_suite_t *suite)
{
    suite->name    = "i2c_eeprom_rw";
    suite->purpose = "eeprom";
    suite->hw_req  = "i2c_eeprom";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_I2C_EEPROM_RW_CASE_COUNT; i++) {
        __i2c_eeprom_rw_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_i2c_eeprom_rw_run,
            (void *)&__i2c_eeprom_rw_cases[i]);
    }
}

void vsf_test_i2c_eeprom_rw_run(const vsf_test_i2c_eeprom_rw_case_t *c)
{
    vsf_i2c_t *i2c = c->suite->i2c;
    uint8_t data_len = c->data_len;

    VSF_TEST_ASSERT(data_len > 0);
    VSF_TEST_ASSERT(data_len <= VSF_TEST_I2C_CASE_MAX_COUNT);

    memset(c->suite->write_buf, 0, sizeof(c->suite->write_buf));
    memset(c->suite->read_buf, 0, sizeof(c->suite->read_buf));
    c->suite->irq_mask = 0;

    /* Init i2c master. */
    vsf_err_t err = vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE,
        .clock_hz   = VSF_TEST_I2C_CLOCK_HZ,
        .isr        = {
            .handler_fn = __i2c_isr,
            .target_ptr = c->suite,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_i2c_enable(i2c));
    vsf_i2c_irq_enable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ERR | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    /* Build write buffer: [mem_addr, payload...]. */
    c->suite->write_buf[0] = c->mem_addr;
    for (uint8_t i = 0; i < data_len; i++) {
        c->suite->write_buf[1 + i] = (uint8_t)(0xA0 + i);
    }

    /* Phase 1: Write [mem_addr, payload] to EEPROM. */
    c->suite->irq_mask = 0;
    err = vsf_i2c_master_request(i2c, c->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        data_len + 1, c->suite->write_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(c->suite, VSF_TEST_I2C_TIMEOUT_MS));

    /* Phase 1.5: ACK poll until EEPROM write cycle completes. */
    VSF_TEST_ASSERT(__eeprom_ack_poll(c->suite, i2c, c->eeprom_addr,
                                      &c->suite->write_buf[0], VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS));

    /* Phase 2a: Set memory address (write phase, no stop). */
    c->suite->irq_mask = 0;
    err = vsf_i2c_master_request(i2c, c->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_NO_STOP | VSF_I2C_CMD_7_BITS,
        1, &c->suite->write_buf[0]);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(c->suite, VSF_TEST_I2C_TIMEOUT_MS));

    /* Phase 2b: Restart, then read data_len bytes. */
    c->suite->irq_mask = 0;
    err = vsf_i2c_master_request(i2c, c->eeprom_addr,
        VSF_I2C_CMD_RESTART | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        data_len, c->suite->read_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(c->suite, VSF_TEST_I2C_TIMEOUT_MS));

    for (uint8_t i = 0; i < data_len; i++) {
        VSF_TEST_ASSERT(c->suite->read_buf[i] == c->suite->write_buf[1 + i]);
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
