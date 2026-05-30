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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#define __VSF_TEST_I2C_CLASS_IMPLEMENT
#include "vsf_test_i2c_eeprom_rw_fifo.h"
/*============================ LOCAL VARIABLES ===============================*/

static volatile vsf_i2c_irq_mask_t __irq_mask;
static uint8_t __write_buf[17];
static uint8_t __read_buf[16];
static volatile bool __done;
static volatile bool __error;
static vsf_i2c_cmd_t __cur_cmd;
static uint_fast16_t __offset;



#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_CLOCK_HZ
#   define VSF_TEST_I2C_CLOCK_HZ                100000
#endif
#ifndef VSF_TEST_I2C_FIFO_TIMEOUT_MS
#   define VSF_TEST_I2C_FIFO_TIMEOUT_MS         2000
#endif
#ifndef VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS
#   define VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS  100
#endif

/*============================ IMPLEMENTATION ================================*/

static void __i2c_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                      vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    vsf_test_suite_t *suite = target_ptr;
    __irq_mask |= irq_mask;
}

static bool __wait_irq(vsf_test_suite_t *suite,
                       vsf_i2c_irq_mask_t check_mask, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (__irq_mask & check_mask) return true;
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

static bool __fifo_write(vsf_i2c_t *i2c, uint16_t addr, vsf_i2c_cmd_t cmd,
                          uint_fast16_t count, uint8_t *buf, uint32_t timeout_ms)
{
    vsf_i2c_cmd_t cur_cmd = 0;
    uint_fast16_t offset  = 0;
    fsm_rt_t result = vsf_i2c_master_fifo_transfer(i2c, addr, cmd,
        count, buf, &cur_cmd, &offset);
    while (result == fsm_rt_on_going && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
        result = vsf_i2c_master_fifo_transfer(i2c, addr, cmd,
            count, buf, &cur_cmd, &offset);
    }
    return result == fsm_rt_cpl;
}

static bool __eeprom_ack_poll(vsf_test_suite_t *suite,
                               vsf_i2c_t *i2c, uint8_t eeprom_addr,
                               uint8_t *dummy_buf, uint32_t max_ms)
{
    while (max_ms-- > 0) {
        __irq_mask = 0;
        vsf_err_t err = vsf_i2c_master_request(i2c, eeprom_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
            1, dummy_buf);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        if (__wait_irq(suite,
            VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR, 10)) {
            if (__irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
                return true;
            }
        }
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

void vsf_test_i2c_eeprom_rw_fifo_run(vsf_test_case_t *tc)
{
    vsf_test_i2c_eeprom_rw_fifo_params_t *p = tc->arg;
    vsf_test_suite_t *suite = tc->suite;
    vsf_i2c_t *i2c = (vsf_i2c_t *)suite->arg;
    uint8_t data_len = p->data_len;

    VSF_TEST_ASSERT(data_len > 0);
    VSF_TEST_ASSERT(data_len <= VSF_TEST_I2C_CASE_MAX_COUNT);

    memset(__write_buf, 0, sizeof(__write_buf));
    memset(__read_buf, 0, sizeof(__read_buf));
    __irq_mask = 0;

    vsf_err_t err = vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE,
        .clock_hz   = VSF_TEST_I2C_CLOCK_HZ,
        .isr        = {
            .handler_fn = __i2c_isr,
            .target_ptr = suite,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_i2c_enable(i2c));
    vsf_i2c_irq_enable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);

    __write_buf[0] = p->mem_addr;
    for (uint8_t i = 0; i < data_len; i++) {
        __write_buf[1 + i] = (uint8_t)(0xA0 + i);
    }

    /* Phase 1: FIFO write [mem_addr, payload] to EEPROM.
     * Uses vsf_i2c_master_fifo_transfer() in polling mode.  This is the
     * key API under test — compare with vsf_i2c_master_request() in the
     * request-based i2c_eeprom_rw suite. */
    VSF_TEST_ASSERT(__fifo_write(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        data_len + 1, __write_buf, VSF_TEST_I2C_FIFO_TIMEOUT_MS));

    /* Phase 1.5: ACK poll until EEPROM write cycle completes. */
    VSF_TEST_ASSERT(__eeprom_ack_poll(suite, i2c, p->eeprom_addr,
                                      &__write_buf[0], VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS));

    /* Phase 2a: Set memory address (write phase, no stop). */
    __irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_NO_STOP | VSF_I2C_CMD_7_BITS,
        1, &__write_buf[0]);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__wait_irq(suite,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR, 1000));
    VSF_TEST_ASSERT(__irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    /* Phase 2b: Read data back (request API). */
    __irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_RESTART | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        data_len, __read_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__wait_irq(suite,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE, VSF_TEST_I2C_FIFO_TIMEOUT_MS));

    for (uint8_t i = 0; i < data_len; i++) {
        VSF_TEST_ASSERT(__read_buf[i] == __write_buf[1 + i]);
    }

    vsf_trace_info("I2C:EEPROM_RW_FIFO:PASS len=%u" VSF_TRACE_CFG_LINEEND,
                   (unsigned)data_len);

    vsf_i2c_irq_disable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c));
    vsf_i2c_fini(i2c);
}

#endif /* VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED */

/* EOF */
