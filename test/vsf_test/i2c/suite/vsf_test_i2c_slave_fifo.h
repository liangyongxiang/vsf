#ifndef __VSF_TEST_I2C_SLAVE_FIFO_H__
#define __VSF_TEST_I2C_SLAVE_FIFO_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_i2c.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT
#   define VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT     1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_i2c_slave_fifo_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_I2C_SLAVE_FIFO_H__ */
/* EOF */
