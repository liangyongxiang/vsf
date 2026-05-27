/*============================ INCLUDES ======================================*/

#include "vsf_test_i2c_slave_fifo.h"

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ
#   define VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ         100000
#endif
#ifndef VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS
#   define VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS       2000
#endif

#define VSF_TEST_I2C_SLAVE_FIFO_ADDR               0x50

/*============================ IMPLEMENTATION ================================*/

static void __master_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                         vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    vsf_test_i2c_slave_fifo_suite_t *suite = (vsf_test_i2c_slave_fifo_suite_t *)target_ptr;
    suite->master_irq_mask |= irq_mask;
    if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        suite->master_done = true;
    }
}

static void __slave_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                        vsf_i2c_irq_mask_t irq_mask)
{
    vsf_test_i2c_slave_fifo_suite_t *suite = (vsf_test_i2c_slave_fifo_suite_t *)target_ptr;
    suite->slave_irq_mask |= irq_mask;

    /* Slave receive via fifo_transfer: read available bytes from RX FIFO. */
    if (irq_mask & VSF_I2C_IRQ_MASK_SLAVE_RX) {
        uint_fast16_t remaining = 16 - suite->slave_rx_offset;
        uint_fast16_t got = vsf_i2c_slave_fifo_transfer(i2c_ptr, false,
            remaining, suite->slave_buf + suite->slave_rx_offset);
        suite->slave_rx_offset += got;
    }
    if (irq_mask & (VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT)) {
        suite->slave_complete = true;
    }
}

static bool __wait_master_done(vsf_test_i2c_slave_fifo_suite_t *suite, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (suite->master_done) return true;
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

static bool __wait_slave_complete(vsf_test_i2c_slave_fifo_suite_t *suite, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (suite->slave_complete) return true;
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

void vsf_test_i2c_slave_fifo_run(void *arg)
{
    vsf_test_i2c_slave_fifo_case_t *c = (vsf_test_i2c_slave_fifo_case_t *)arg;
    vsf_i2c_t *master_i2c = c->suite->master_i2c;
    vsf_i2c_t *slave_i2c  = c->suite->slave_i2c;
    vsf_test_i2c_slave_fifo_suite_t *suite = c->suite;

    /* Zero all per-run state. */
    uintptr_t base = (uintptr_t)&suite->master_irq_mask;
    uintptr_t end  = (uintptr_t)&suite->slave_complete + sizeof(suite->slave_complete);
    memset((void *)base, 0, end - base);

    /* ---- Init slave (fifo-driven RX) ---- */
    vsf_err_t err = vsf_i2c_init(slave_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_I2C_MODE_SLAVE | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ,
        .slave_addr = VSF_TEST_I2C_SLAVE_FIFO_ADDR,
        .isr        = {
            .handler_fn = __slave_isr,
            .target_ptr = suite,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_i2c_enable(slave_i2c));
    vsf_i2c_irq_enable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    /* ---- Init master (request-driven) ---- */
    err = vsf_i2c_init(master_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_FIFO_CLOCK_HZ,
        .isr        = {
            .handler_fn = __master_isr,
            .target_ptr = suite,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_i2c_enable(master_i2c));
    vsf_i2c_irq_enable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);

    /* ---- Slave receive via FIFO (master writes) ---- */
    for (uint8_t i = 0; i < 16; i++) {
        suite->master_buf[i] = (uint8_t)(0xA0 + i);
        suite->slave_buf[i] = 0;
    }
    suite->master_done     = false;
    suite->slave_complete  = false;
    suite->slave_rx_offset = 0;

    err = vsf_i2c_master_request(master_i2c, VSF_TEST_I2C_SLAVE_FIFO_ADDR,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        16, suite->master_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    VSF_TEST_ASSERT(__wait_master_done(suite, VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS));
    VSF_TEST_ASSERT(__wait_slave_complete(suite, VSF_TEST_I2C_SLAVE_FIFO_TIMEOUT_MS));

    for (uint8_t i = 0; i < 16; i++) {
        VSF_TEST_ASSERT(suite->slave_buf[i] == suite->master_buf[i]);
    }

    vsf_trace_info("I2C:SLAVE_FIFO:RX:PASS" VSF_TRACE_CFG_LINEEND);

    /* ---- Cleanup ---- */
    vsf_i2c_irq_disable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);
    vsf_i2c_irq_disable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    while (fsm_rt_cpl != vsf_i2c_disable(master_i2c));
    while (fsm_rt_cpl != vsf_i2c_disable(slave_i2c));
    vsf_i2c_fini(master_i2c);
    vsf_i2c_fini(slave_i2c);
}

#endif /* VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED */

/* EOF */
