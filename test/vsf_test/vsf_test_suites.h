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
 ****************************************************************************/

// Single header that aggregates all vsf_test peripheral suite headers.
// Every main() / entry point that wants to run all tests should include this
// instead of listing individual peripheral headers.

#ifndef __VSF_TEST_SUITES_H__
#define __VSF_TEST_SUITES_H__

#include "usart/vsf_test_usart.h"
#include "gpio/vsf_test_gpio.h"
#include "i2c/vsf_test_i2c.h"
#include "rtc/vsf_test_rtc.h"
#include "flash/vsf_test_flash.h"
#include "wdt/vsf_test_wdt.h"
#include "adc/vsf_test_adc.h"
#include "pwm/vsf_test_pwm.h"
#include "timer/vsf_test_timer.h"
#include "spi/vsf_test_spi.h"
#include "rng/vsf_test_rng.h"
#include "dma/vsf_test_dma.h"

typedef union {
#if VSF_TEST_ADC_ENABLE == ENABLED
    vsf_test_adc_data_t adc;
#endif
#if VSF_TEST_DMA_ENABLE == ENABLED
    vsf_test_dma_data_t dma;
#endif
#if VSF_TEST_FLASH_ENABLE == ENABLED
    vsf_test_flash_data_t flash;
#endif
#if VSF_TEST_GPIO_ENABLE == ENABLED
    vsf_test_gpio_data_t gpio;
#endif
#if VSF_TEST_I2C_ENABLE == ENABLED
    vsf_test_i2c_data_t i2c;
#endif
#if VSF_TEST_RTC_ENABLE == ENABLED
    vsf_test_rtc_data_t rtc;
#endif
#if VSF_TEST_SPI_ENABLE == ENABLED
    vsf_test_spi_data_t spi;
#endif
#if VSF_TEST_TIMER_ENABLE == ENABLED
    vsf_test_timer_data_t timer;
#endif
#if VSF_TEST_USART_ENABLE == ENABLED
    vsf_test_usart_data_t usart;
#endif
} vsf_test_suite_data_t;

/*============================ TYPES =========================================*/

typedef struct {
#if VSF_TEST_ADC_ENABLE == ENABLED
    vsf_test_adc_cases_t adc;
#endif
#if VSF_TEST_DMA_ENABLE == ENABLED
    vsf_test_dma_cases_t dma;
#endif
#if VSF_TEST_FLASH_ENABLE == ENABLED
    vsf_test_flash_cases_t flash;
#endif
#if VSF_TEST_GPIO_ENABLE == ENABLED
    vsf_test_gpio_cases_t gpio;
#endif
#if VSF_TEST_I2C_ENABLE == ENABLED
    vsf_test_i2c_cases_t i2c;
#endif
#if VSF_TEST_PWM_ENABLE == ENABLED
    vsf_test_pwm_cases_t pwm;
#endif
#if VSF_TEST_RNG_ENABLE == ENABLED
    vsf_test_rng_cases_t rng;
#endif
#if VSF_TEST_RTC_ENABLE == ENABLED
    vsf_test_rtc_cases_t rtc;
#endif
#if VSF_TEST_SPI_ENABLE == ENABLED
    vsf_test_spi_cases_t spi;
#endif
#if VSF_TEST_TIMER_ENABLE == ENABLED
    vsf_test_timer_cases_t timer;
#endif
#if VSF_TEST_USART_ENABLE == ENABLED
    vsf_test_usart_cases_t usart;
#endif
#if VSF_TEST_WDT_ENABLE == ENABLED
    vsf_test_wdt_cases_t wdt;
#endif
} vsf_test_all_cases_t;

typedef struct {
#if VSF_TEST_ADC_ENABLE == ENABLED
    vsf_test_adc_params_t adc;
#endif
#if VSF_TEST_DMA_ENABLE == ENABLED
    vsf_test_dma_params_t dma;
#endif
#if VSF_TEST_FLASH_ENABLE == ENABLED
    vsf_test_flash_params_t flash;
#endif
#if VSF_TEST_GPIO_ENABLE == ENABLED
    vsf_test_gpio_params_t gpio;
#endif
#if VSF_TEST_I2C_ENABLE == ENABLED
    vsf_test_i2c_params_t i2c;
#endif
#if VSF_TEST_PWM_ENABLE == ENABLED
    vsf_test_pwm_params_t pwm;
#endif
#if VSF_TEST_RNG_ENABLE == ENABLED
    vsf_test_rng_params_t rng;
#endif
#if VSF_TEST_RTC_ENABLE == ENABLED
    vsf_test_rtc_params_t rtc;
#endif
#if VSF_TEST_SPI_ENABLE == ENABLED
    vsf_test_spi_params_t spi;
#endif
#if VSF_TEST_TIMER_ENABLE == ENABLED
    vsf_test_timer_params_t timer;
#endif
#if VSF_TEST_USART_ENABLE == ENABLED
    vsf_test_usart_params_t usart;
#endif
#if VSF_TEST_WDT_ENABLE == ENABLED
    vsf_test_wdt_params_t wdt;
#endif
} vsf_test_all_params_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_test_suite_data_t vsf_test_suite_data;
extern const vsf_test_suite_t vsf_test_suite_list[];
extern uint8_t vsf_test_suite_count;

#endif // __VSF_TEST_SUITES_H__
