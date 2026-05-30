#ifndef __VSF_TEST_I2C_SLAVE_H__
#define __VSF_TEST_I2C_SLAVE_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_i2c.h"
#ifndef VSF_TEST_I2C_SLAVE_MASTER_BUF_SIZE
#   define VSF_TEST_I2C_SLAVE_MASTER_BUF_SIZE        16
#endif
#ifndef VSF_TEST_I2C_SLAVE_SLAVE_BUF_SIZE
#   define VSF_TEST_I2C_SLAVE_SLAVE_BUF_SIZE        16
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_I2C_SLAVE_CASE_COUNT
#   define VSF_TEST_I2C_SLAVE_CASE_COUNT     1
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
typedef struct {
    i2c_slave_state_t i2c_slave_state;
} vsf_test_i2c_slave_var_t;
#endif
/*============================ PROTOTYPES ====================================*/

void vsf_test_i2c_slave_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_I2C_SLAVE_H__ */
/* EOF */
