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
#include "vsf_test_i2c_eeprom_page.h"
#include "vsf_test_suites.h"
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED

typedef enum {
    I2C_POLL_COMPLETE,
    I2C_POLL_ERR,
    I2C_POLL_TIMEOUT,
} __i2c_poll_result_t;

static void __i2c_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                      vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    vsf_test_suite_t *suite = target_ptr;
    vsf_test_suite_data.i2c_eeprom_page.irq_mask |= irq_mask;
}

static __i2c_poll_result_t __i2c_wait_result(vsf_test_suite_t *suite,
                                              uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (vsf_test_suite_data.i2c_eeprom_page.irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            return I2C_POLL_COMPLETE;
        }
        if (vsf_test_suite_data.i2c_eeprom_page.irq_mask & VSF_I2C_IRQ_MASK_MASTER_ERR) {
            return I2C_POLL_ERR;
        }
        vsf_test_busy_wait_ms(1);
    }
    return I2C_POLL_TIMEOUT;
}

static bool __i2c_wait_complete(vsf_test_suite_t *suite, uint32_t timeout_ms)
{
    return __i2c_wait_result(suite, timeout_ms) == I2C_POLL_COMPLETE;
}

static bool __eeprom_ack_poll(vsf_test_suite_t *suite,
                               vsf_i2c_t *i2c, uint8_t eeprom_addr,
                               uint8_t *dummy_buf, uint32_t max_ms)
{
    while (max_ms-- > 0) {
        vsf_test_suite_data.i2c_eeprom_page.irq_mask = 0;
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
        vsf_test_busy_wait_ms(1);
    }
    return false;
}



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
#ifndef VSF_TEST_I2C_EEPROM_PAGE_SIZE
#   define VSF_TEST_I2C_EEPROM_PAGE_SIZE        32
#endif

/*============================ IMPLEMENTATION ================================*/

void vsf_test_i2c_eeprom_page_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_i2c_eeprom_page_params_t *p = tc->arg;
    vsf_i2c_t *i2c = (vsf_i2c_t *)fixture;
    uint8_t data_len = p->data_len;
    uint8_t mem_addr = p->mem_addr;

    VSF_TEST_ASSERT(data_len > 0);
    VSF_TEST_ASSERT(data_len <= VSF_TEST_I2C_CASE_MAX_COUNT);

    memset(vsf_test_suite_data.i2c_eeprom_page.write_buf, 0, sizeof(vsf_test_suite_data.i2c_eeprom_page.write_buf));
    memset(vsf_test_suite_data.i2c_eeprom_page.read_buf, 0, sizeof(vsf_test_suite_data.i2c_eeprom_page.read_buf));
    vsf_test_suite_data.i2c_eeprom_page.irq_mask = 0;

    /* Init i2c master at standard 100kHz. */
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
        VSF_I2C_IRQ_MASK_MASTER_ERR | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    /* Build write buffer: [mem_addr, payload...]. */
    vsf_test_suite_data.i2c_eeprom_page.write_buf[0] = mem_addr;
    for (uint8_t i = 0; i < data_len; i++) {
        vsf_test_suite_data.i2c_eeprom_page.write_buf[1 + i] = (uint8_t)(0xB0 + i);
    }

    /* Phase 1: Write [mem_addr, payload] to EEPROM. */
    vsf_test_suite_data.i2c_eeprom_page.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        data_len + 1, vsf_test_suite_data.i2c_eeprom_page.write_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(suite, VSF_TEST_I2C_TIMEOUT_MS));

    /* Phase 1.5: ACK poll until EEPROM write cycle completes. */
    VSF_TEST_ASSERT(__eeprom_ack_poll(suite, i2c, p->eeprom_addr,
                                      &vsf_test_suite_data.i2c_eeprom_page.write_buf[0], VSF_TEST_I2C_EEPROM_ACK_POLL_MAX_MS));

    /* Phase 2a: Set memory address (write phase, no stop). */
    vsf_test_suite_data.i2c_eeprom_page.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_NO_STOP | VSF_I2C_CMD_7_BITS,
        1, &vsf_test_suite_data.i2c_eeprom_page.write_buf[0]);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(suite, VSF_TEST_I2C_TIMEOUT_MS));

    /* Phase 2b: Restart, then read data_len bytes. */
    vsf_test_suite_data.i2c_eeprom_page.irq_mask = 0;
    err = vsf_i2c_master_request(i2c, p->eeprom_addr,
        VSF_I2C_CMD_RESTART | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        data_len, vsf_test_suite_data.i2c_eeprom_page.read_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(__i2c_wait_complete(suite, VSF_TEST_I2C_TIMEOUT_MS));

    for (uint8_t i = 0; i < data_len; i++) {
        VSF_TEST_ASSERT(vsf_test_suite_data.i2c_eeprom_page.read_buf[i] == vsf_test_suite_data.i2c_eeprom_page.write_buf[1 + i]);
    }

    vsf_trace_info("I2C:EEPROM_PAGE:PASS addr=0x%02X len=%u"
                   VSF_TRACE_CFG_LINEEND,
                   (unsigned)mem_addr, (unsigned)data_len);

    vsf_i2c_irq_disable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ERR | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c));
    vsf_i2c_fini(i2c);
}

#endif /* VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED */

/* EOF */