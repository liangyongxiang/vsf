#ifndef __VSF_TEST_I2C_SLAVE_H__
#define __VSF_TEST_I2C_SLAVE_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_i2c.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_SLAVE_CASE_COUNT
#   define VSF_TEST_I2C_SLAVE_CASE_COUNT     1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_i2c_slave_run(vsf_test_case_t *tc);

#endif /* __VSF_TEST_I2C_SLAVE_H__ */
/* EOF */
