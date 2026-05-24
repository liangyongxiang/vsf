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
#include "vsf_test_i2c_bus_scan.h"

#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_i2c_bus_scan_case_t __i2c_bus_scan_cases[] = {
    { .scl_pin = 18, .sda_pin = 19, .gpio_i2c = NULL },
    { .scl_pin = 21, .sda_pin = 20, .gpio_i2c = NULL },
};

/*============================ LOCAL FUNCTIONS ===============================*/

static void __bus_scan_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
                            vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    vsf_test_i2c_bus_scan_suite_t *suite = (vsf_test_i2c_bus_scan_suite_t *)target_ptr;
    suite->irq_mask = irq_mask;
}

static int __bus_scan_once(vsf_test_i2c_bus_scan_suite_t *suite,
                            vsf_gpio_i2c_t *gpio_i2c, uint8_t scl, uint8_t sda)
{
    gpio_i2c->scl_pin = scl;
    gpio_i2c->sda_pin = sda;

    vsf_i2c_t *i2c = (vsf_i2c_t *)gpio_i2c;
    vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode = 0, .clock_hz = 100000,
        .isr  = {.handler_fn = __bus_scan_isr, .target_ptr = suite},
    });
    vsf_i2c_enable(i2c);
    vsf_i2c_irq_enable(i2c, VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                             | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    int found = 0;
    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        suite->irq_mask = 0;
        vsf_i2c_master_request(i2c, addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP
            | VSF_I2C_CMD_7_BITS | VSF_I2C_CMD_WRITE,
            0, NULL);
        if (!(suite->irq_mask & VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK)) {
            found++;
        }
    }

    vsf_i2c_irq_disable(i2c,
        VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
        | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
    while (fsm_rt_cpl != vsf_i2c_disable(i2c));
    vsf_i2c_fini(i2c);

    return found;
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_i2c_bus_scan_add_cases(vsf_test_i2c_bus_scan_suite_t *suite)
{
    suite->name    = "i2c_bus_scan";
    suite->purpose = "bus_scan";
    suite->hw_req  = NULL;   // no external fixture needed
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    __i2c_bus_scan_cases[0].gpio_i2c = suite->gpio_i2c0;
    __i2c_bus_scan_cases[1].gpio_i2c = suite->gpio_i2c1;
    for (uint8_t i = 0; i < 2; i++) {
        __i2c_bus_scan_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_i2c_bus_scan_run,
            (void *)&__i2c_bus_scan_cases[i]);
    }
}

void vsf_test_i2c_bus_scan_run(const vsf_test_i2c_bus_scan_case_t *c)
{
    vsf_gpio_i2c_t *gpio_i2c = c->gpio_i2c;

    uint8_t scl = c->scl_pin;
    uint8_t sda = c->sda_pin;

    vsf_trace_info("--- I2C scan (SCL=GP%u, SDA=GP%u) ---"
                   VSF_TRACE_CFG_LINEEND, scl, sda);
    int found = __bus_scan_once(c->suite, gpio_i2c, scl, sda);
    vsf_trace_info("  Devices found: %d" VSF_TRACE_CFG_LINEEND, found);

    bool swapped = false;
    if (found == 0) {
        vsf_trace_info("  No response.  Trying swapped SCL/SDA..."
                       VSF_TRACE_CFG_LINEEND);
        found = __bus_scan_once(c->suite, gpio_i2c, sda, scl);
        vsf_trace_info("  Devices found (swapped): %d" VSF_TRACE_CFG_LINEEND,
                       found);
        swapped = (found > 0);
    }

    if (swapped) {
        vsf_trace_info("[I2C] SCL/SDA appear swapped.  "
                       "Use (GP%u=SDA, GP%u=SCL)."
                       VSF_TRACE_CFG_LINEEND, scl, sda);
    } else if (found == 0) {
        vsf_trace_info("[I2C] No device found on either pin order.  "
                       "Check pull-ups and power."
                       VSF_TRACE_CFG_LINEEND);
    }

    VSF_TEST_ASSERT(found > 0);
}

#endif /* VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED */

/* EOF */
