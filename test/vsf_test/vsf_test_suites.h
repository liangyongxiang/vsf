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
    struct {
    #if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
        vsf_test_adc_stream_data_t adc_stream;
    #endif
    } adc;
#endif
#if VSF_TEST_DMA_ENABLE == ENABLED
    struct {
    #if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
        vsf_test_dma_mem2mem_irq_data_t dma_mem2mem_irq;
    #endif
    #if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
        vsf_test_dma_scatter_gather_data_t dma_scatter_gather;
    #endif
    } dma;
#endif
#if VSF_TEST_FLASH_ENABLE == ENABLED
    struct {
    #if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
        vsf_test_flash_boundary_data_t flash_boundary;
    #endif
    #if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
        vsf_test_flash_erase_program_read_data_t flash_erase_program_read;
    #endif
    } flash;
#endif
#if VSF_TEST_GPIO_ENABLE == ENABLED
    struct {
    #if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
        vsf_test_gpio_concurrent_prio_data_t gpio_concurrent_prio;
    #endif
    #if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
        vsf_test_gpio_exti_data_t gpio_exti;
    #endif
    #if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
        vsf_test_gpio_irq_latency_data_t gpio_irq_latency;
    #endif
    #if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
        vsf_test_gpio_irq_lifecycle_data_t gpio_irq_lifecycle;
    #endif
    } gpio;
#endif
#if VSF_TEST_I2C_ENABLE == ENABLED
    struct {
    #if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
        vsf_test_i2c_bus_scan_data_t i2c_bus_scan;
    #endif
    #if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
        vsf_test_i2c_eeprom_page_data_t i2c_eeprom_page;
    #endif
    #if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
        vsf_test_i2c_eeprom_rw_data_t i2c_eeprom_rw;
    #endif
    #if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
        vsf_test_i2c_eeprom_rw_fifo_data_t i2c_eeprom_rw_fifo;
    #endif
    #if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
        vsf_test_i2c_slave_data_t i2c_slave;
    #endif
    #if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
        vsf_test_i2c_slave_fifo_data_t i2c_slave_fifo;
    #endif
    } i2c;
#endif
#if VSF_TEST_RTC_ENABLE == ENABLED
    struct {
    #if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
        vsf_test_rtc_alarm_data_t rtc_alarm;
    #endif
    } rtc;
#endif
#if VSF_TEST_SPI_ENABLE == ENABLED
    struct {
    #if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
        vsf_test_spi_async_data_t spi_async;
    #endif
    } spi;
#endif
#if VSF_TEST_TIMER_ENABLE == ENABLED
    struct {
    #if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
        vsf_test_timer_async_data_t timer_async;
    #endif
    #if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
        vsf_test_timer_oneshot_data_t timer_oneshot;
    #endif
    #if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
        vsf_test_timer_periodic_data_t timer_periodic;
    #endif
    } timer;
#endif
#if VSF_TEST_USART_ENABLE == ENABLED
    struct {
    #if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
        vsf_test_usart_request_rx_irq_data_t usart_request_rx_irq;
    #endif
    #if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
        vsf_test_usart_request_tx_irq_data_t usart_request_tx_irq;
    #endif
    #if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
        vsf_test_usart_rx_bulk_irq_data_t usart_rx_bulk_irq;
    #endif
    #if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
        vsf_test_usart_rx_data_data_t usart_rx_data;
    #endif
    #if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
        vsf_test_usart_rx_fifo_irq_data_t usart_rx_fifo_irq;
    #endif
    #if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
        vsf_test_usart_rx_fifo_threshold_data_t usart_rx_fifo_threshold;
    #endif
    #if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
        vsf_test_usart_tx_fifo_irq_data_t usart_tx_fifo_irq;
    #endif
    } usart;
#endif
} vsf_test_suite_data_t;

/*============================ TYPES =========================================*/

typedef struct {
    vsf_test_adc_cases_t adc;
    vsf_test_dma_cases_t dma;
    vsf_test_flash_cases_t flash;
    vsf_test_gpio_cases_t gpio;
    vsf_test_i2c_cases_t i2c;
    vsf_test_pwm_cases_t pwm;
    vsf_test_rng_cases_t rng;
    vsf_test_rtc_cases_t rtc;
    vsf_test_spi_cases_t spi;
    vsf_test_timer_cases_t timer;
    vsf_test_usart_cases_t usart;
    vsf_test_wdt_cases_t wdt;
} vsf_test_all_cases_t;

typedef struct {
    vsf_test_adc_params_t adc;
    vsf_test_dma_params_t dma;
    vsf_test_flash_params_t flash;
    vsf_test_gpio_params_t gpio;
    vsf_test_i2c_params_t i2c;
    vsf_test_pwm_params_t pwm;
    vsf_test_rng_params_t rng;
    vsf_test_rtc_params_t rtc;
    vsf_test_spi_params_t spi;
    vsf_test_timer_params_t timer;
    vsf_test_usart_params_t usart;
    vsf_test_wdt_params_t wdt;
} vsf_test_all_params_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_test_suite_data_t vsf_test_suite_data;
extern const vsf_test_suite_t vsf_test_suite_list[];
extern uint8_t vsf_test_suite_count;

#endif // __VSF_TEST_SUITES_H__
