/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
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

#ifndef __TEST_I2C_EEPROM_RW_H__
#define __TEST_I2C_EEPROM_RW_H__

#include "../vsf_test_i2c.h"
#ifndef VSF_TEST_I2C_EEPROM_RW_WRITE_BUF_SIZE
#   define VSF_TEST_I2C_EEPROM_RW_WRITE_BUF_SIZE        17
#endif
#ifndef VSF_TEST_I2C_EEPROM_RW_READ_BUF_SIZE
#   define VSF_TEST_I2C_EEPROM_RW_READ_BUF_SIZE        16
#endif

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t irq_mask;
    uint8_t write_buf[VSF_TEST_I2C_EEPROM_RW_WRITE_BUF_SIZE];
    uint8_t read_buf[VSF_TEST_I2C_EEPROM_RW_READ_BUF_SIZE];
} vsf_test_i2c_eeprom_rw_var_t;
#endif

#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_i2c_eeprom_rw_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED */

#endif /* __TEST_I2C_EEPROM_RW_H__ */
/* EOF */
