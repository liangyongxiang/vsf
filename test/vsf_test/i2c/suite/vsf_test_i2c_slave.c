/*============================ INCLUDES ======================================*/

#include "vsf_test_i2c_slave.h"
/*============================ LOCAL VARIABLES ===============================*/

typedef struct {
    volatile vsf_i2c_irq_mask_t master_irq_mask;
    volatile vsf_i2c_irq_mask_t slave_irq_mask;
    uint8_t master_buf[16];
    uint8_t slave_buf[16];
} __i2c_slave_state_t;

static __i2c_slave_state_t __i2c_slave_state;



#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_SLAVE_CLOCK_HZ
#   define VSF_TEST_I2C_SLAVE_CLOCK_HZ         100000
#endif
#ifndef VSF_TEST_I2C_SLAVE_TIMEOUT_MS
#   define VSF_TEST_I2C_SLAVE_TIMEOUT_MS       1000
#endif

#define VSF_TEST_I2C_SLAVE_ADDR               0x50

/*============================ IMPLEMENTATION ================================*/

static void __master_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                         vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    __i2c_slave_state_t *st = (__i2c_slave_state_t *)target_ptr;
    st->master_irq_mask |= irq_mask;
}

static void __slave_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                        vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    __i2c_slave_state_t *st = (__i2c_slave_state_t *)target_ptr;
    st->slave_irq_mask |= irq_mask;
}

static bool __wait_master_complete(__i2c_slave_state_t *st, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (st->master_irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            return true;
        }
        if (st->master_irq_mask & (VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                    | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT
                                    | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST)) {
            return false;
        }
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

static bool __wait_slave_complete(__i2c_slave_state_t *st, uint32_t timeout_ms)
{
    while (timeout_ms-- > 0) {
        if (st->slave_irq_mask & VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE) {
            return true;
        }
        if (st->slave_irq_mask & VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT) {
            return true;
        }
        vsf_test_busy_wait_ms(1);
    }
    return false;
}

void vsf_test_i2c_slave_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_i2c_slave_params_t *p = tc->arg;
    void **handles = (void **)fixture;
    vsf_i2c_t *master_i2c = (vsf_i2c_t *)handles[0];
    vsf_i2c_t *slave_i2c  = (vsf_i2c_t *)handles[1];
    __i2c_slave_state_t *st = &__i2c_slave_state;

    memset(st, 0, sizeof(*st));

    /* ---- Init slave first ---- */
    vsf_err_t err = vsf_i2c_init(slave_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_I2C_MODE_SLAVE | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_CLOCK_HZ,
        .slave_addr = VSF_TEST_I2C_SLAVE_ADDR,
        .isr        = {
            .handler_fn = __slave_isr,
            .target_ptr = st,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_i2c_enable(slave_i2c));
    vsf_i2c_irq_enable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX | VSF_I2C_IRQ_MASK_SLAVE_TX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    /* ---- Init master ---- */
    err = vsf_i2c_init(master_i2c, &(vsf_i2c_cfg_t){
        .mode       = VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS | VSF_I2C_SPEED_STANDARD_MODE,
        .clock_hz   = VSF_TEST_I2C_SLAVE_CLOCK_HZ,
        .isr        = {
            .handler_fn = __master_isr,
            .target_ptr = st,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_i2c_enable(master_i2c));
    vsf_i2c_irq_enable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);

    /* ---- Case: Slave receive (master writes) ---- */
    for (uint8_t i = 0; i < 16; i++) {
        st->master_buf[i] = (uint8_t)(0xA0 + i);
        st->slave_buf[i] = 0;
    }

    st->master_irq_mask = 0;
    st->slave_irq_mask  = 0;

    /* Slave prepares to receive */
    err = vsf_i2c_slave_request(slave_i2c, false, 16, st->slave_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Small delay to ensure slave is ready */
    vsf_test_busy_wait_ms(5);

    /* Master writes to slave address */
    err = vsf_i2c_master_request(master_i2c, VSF_TEST_I2C_SLAVE_ADDR,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS,
        16, st->master_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    VSF_TEST_ASSERT(__wait_master_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));
    VSF_TEST_ASSERT(__wait_slave_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));

    for (uint8_t i = 0; i < 16; i++) {
        VSF_TEST_ASSERT(st->slave_buf[i] == st->master_buf[i]);
    }

    vsf_trace_info("I2C:SLAVE:RX:PASS" VSF_TRACE_CFG_LINEEND);

    /* ---- Case: Slave transmit (master reads) ---- */
    for (uint8_t i = 0; i < 16; i++) {
        st->slave_buf[i] = (uint8_t)(0xB0 + i);
        st->master_buf[i] = 0;
    }

    st->master_irq_mask = 0;
    st->slave_irq_mask  = 0;

    /* Slave prepares to transmit */
    err = vsf_i2c_slave_request(slave_i2c, true, 16, st->slave_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_test_busy_wait_ms(5);

    /* Master reads from slave address */
    err = vsf_i2c_master_request(master_i2c, VSF_TEST_I2C_SLAVE_ADDR,
        VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS,
        16, st->master_buf);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    VSF_TEST_ASSERT(__wait_master_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));
    VSF_TEST_ASSERT(__wait_slave_complete(st, VSF_TEST_I2C_SLAVE_TIMEOUT_MS));

    for (uint8_t i = 0; i < 16; i++) {
        VSF_TEST_ASSERT(st->master_buf[i] == st->slave_buf[i]);
    }

    vsf_trace_info("I2C:SLAVE:TX:PASS" VSF_TRACE_CFG_LINEEND);

    /* ---- Cleanup ---- */
    vsf_i2c_irq_disable(master_i2c,
        VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_MASTER_ERR);
    vsf_i2c_irq_disable(slave_i2c,
        VSF_I2C_IRQ_MASK_SLAVE_RX | VSF_I2C_IRQ_MASK_SLAVE_TX
        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE | VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT);

    while (fsm_rt_cpl != vsf_i2c_disable(master_i2c));
    while (fsm_rt_cpl != vsf_i2c_disable(slave_i2c));
    vsf_i2c_fini(master_i2c);
    vsf_i2c_fini(slave_i2c);
}

#endif /* VSF_TEST_I2C_SLAVE_ENABLE == ENABLED */

/* EOF */
