#ifndef __VSF_TEST_I2C_SLAVE_FIFO_H__
#define __VSF_TEST_I2C_SLAVE_FIFO_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"
#ifndef VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE
#   define VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE        16
#endif
#ifndef VSF_TEST_I2C_SLAVE_FIFO_SLAVE_BUF_SIZE
#   define VSF_TEST_I2C_SLAVE_FIFO_SLAVE_BUF_SIZE        16
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT
#   define VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT     1
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t master_irq_mask;
    volatile vsf_i2c_irq_mask_t slave_irq_mask;
    uint8_t master_buf[VSF_TEST_I2C_SLAVE_FIFO_MASTER_BUF_SIZE];
    uint8_t slave_buf[VSF_TEST_I2C_SLAVE_FIFO_SLAVE_BUF_SIZE];
    volatile uint_fast16_t slave_rx_offset;
    volatile bool master_done;
    volatile bool slave_complete;
} vsf_test_i2c_slave_fifo_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_i2c_slave_fifo_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_I2C_SLAVE_FIFO_H__ */
/* EOF */
