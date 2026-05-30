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

#include "test/vsf_test/usart/vsf_test_usart.h"
#include "test/vsf_test/gpio/vsf_test_gpio.h"
#include "test/vsf_test/i2c/vsf_test_i2c.h"
#include "test/vsf_test/rtc/vsf_test_rtc.h"
#include "test/vsf_test/flash/vsf_test_flash.h"
#include "test/vsf_test/wdt/vsf_test_wdt.h"
#include "test/vsf_test/adc/vsf_test_adc.h"
#include "test/vsf_test/pwm/vsf_test_pwm.h"
#include "test/vsf_test/timer/vsf_test_timer.h"
#include "test/vsf_test/spi/vsf_test_spi.h"
#include "test/vsf_test/spi/suite/vsf_test_spi_async.h"
#include "test/vsf_test/rng/vsf_test_rng.h"
#include "test/vsf_test/dma/vsf_test_dma.h"
#include "test/vsf_test/dma/suite/vsf_test_dma_scatter_gather.h"

#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
typedef struct {
    volatile bool completed;
} vsf_test_adc_stream_var_t;
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
typedef struct {
    volatile bool irq_fired;
} vsf_test_dma_mem2mem_irq_var_t;
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
typedef struct {
    uint8_t sg_src_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 8];
    uint8_t sg_dst_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 8];
    volatile bool sg_done;
} vsf_test_dma_scatter_gather_var_t;
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
typedef struct {
    uint8_t write_buf[512];
    uint8_t read_buf[512];
} vsf_test_flash_boundary_var_t;
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
typedef struct {
    uint8_t write_buf[512];
    uint8_t read_buf[512];
} vsf_test_flash_erase_program_read_var_t;
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
typedef struct {
    vsf_gpio_pin_mask_t out_mask;
    uint32_t period_us;
    volatile uint32_t callback_toggles;
    volatile uint32_t main_toggles;
} vsf_test_gpio_concurrent_prio_var_t;
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
typedef struct {
    vsf_gpio_pin_mask_t expected_pin;
    volatile uint32_t count;
    bool disable_on_fire;
} vsf_test_gpio_exti_var_t;
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
typedef struct {
    vsf_gpio_pin_mask_t expected_pin;
    volatile vsf_systimer_tick_t isr_tick;
    volatile bool fired;
    vsf_systimer_tick_t trigger_tick;
} vsf_test_gpio_irq_latency_var_t;
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
typedef struct {
    volatile uint32_t lifecycle_count;
    vsf_gpio_pin_mask_t lifecycle_pin;
} vsf_test_gpio_irq_lifecycle_var_t;
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t irq_mask;
} vsf_test_i2c_bus_scan_var_t;
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t irq_mask;
    uint8_t write_buf[17];
    uint8_t read_buf[16];
} vsf_test_i2c_eeprom_page_var_t;
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t irq_mask;
    uint8_t write_buf[17];
    uint8_t read_buf[16];
} vsf_test_i2c_eeprom_rw_var_t;
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t irq_mask;
    uint8_t write_buf[17];
    uint8_t read_buf[16];
    volatile bool done;
    volatile bool error;
    vsf_i2c_cmd_t cur_cmd;
    uint_fast16_t offset;
} vsf_test_i2c_eeprom_rw_fifo_var_t;
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
typedef struct {
    i2c_slave_state_t i2c_slave_state;
} vsf_test_i2c_slave_var_t;
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
typedef struct {
    volatile vsf_i2c_irq_mask_t master_irq_mask;
    volatile vsf_i2c_irq_mask_t slave_irq_mask;
    uint8_t master_buf[16];
    uint8_t slave_buf[16];
    volatile uint_fast16_t slave_rx_offset;
    volatile bool master_done;
    volatile bool slave_complete;
} vsf_test_i2c_slave_fifo_var_t;
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
typedef struct {
    volatile bool alarm_triggered;
} vsf_test_rtc_alarm_var_t;
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
typedef struct {
    uint8_t spi_async_tx_buf[VSF_TEST_SPI_ASYNC_MAX_DATA_LEN];
    uint8_t spi_async_rx_buf[VSF_TEST_SPI_ASYNC_MAX_DATA_LEN];
} vsf_test_spi_async_var_t;
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
typedef struct {
    volatile uint8_t counter;
} vsf_test_timer_async_var_t;
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
typedef struct {
    volatile bool fired;
} vsf_test_timer_oneshot_var_t;
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
typedef struct {
    volatile uint32_t counter;
} vsf_test_timer_periodic_var_t;
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
typedef struct {
    volatile bool req_rx_cpl;
    volatile uint32_t req_rx_irq_count;
    uint8_t req_rx_buf[256];
} vsf_test_usart_request_rx_irq_var_t;
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
typedef struct {
    volatile bool req_tx_cpl;
    volatile uint32_t req_tx_irq_count;
} vsf_test_usart_request_tx_irq_var_t;
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
typedef struct {
    uint8_t rx_bulk_irq_buf[4096];
    volatile bool done;
    uint8_t *dst;
    volatile uint32_t isr_count;
    uint_fast16_t received;
    uint_fast16_t target;
} vsf_test_usart_rx_bulk_irq_var_t;
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
typedef struct {
    uint8_t rx_data_buf[4096];
} vsf_test_usart_rx_data_var_t;
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
typedef struct {
    volatile uint32_t isr_count;
    uint_fast16_t received;
    uint_fast16_t target;
    uint8_t *dst;
    volatile bool done;
} vsf_test_usart_rx_fifo_irq_var_t;
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
typedef struct {
    uint8_t rx_fifo_threshold_buf[64];
    volatile bool threshold_fired;
    volatile uint32_t bytes_at_threshold;
    volatile uint32_t isr_count;
    volatile bool done;
    uint8_t *dst;
    uint32_t target;
    volatile uint32_t received;
} vsf_test_usart_rx_fifo_threshold_var_t;
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
typedef struct {
    volatile uint32_t isr_count;
    uint_fast16_t remaining;
    const uint8_t *src;
    volatile bool done;
} vsf_test_usart_tx_fifo_irq_var_t;
#endif

/*============================ SCENARIO STATE UNION ==========================*/

typedef union {
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    vsf_test_adc_stream_var_t adc_stream;
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    vsf_test_dma_mem2mem_irq_var_t dma_mem2mem_irq;
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    vsf_test_dma_scatter_gather_var_t dma_scatter_gather;
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    vsf_test_flash_boundary_var_t flash_boundary;
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    vsf_test_flash_erase_program_read_var_t flash_erase_program_read;
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    vsf_test_gpio_concurrent_prio_var_t gpio_concurrent_prio;
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    vsf_test_gpio_exti_var_t gpio_exti;
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    vsf_test_gpio_irq_latency_var_t gpio_irq_latency;
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    vsf_test_gpio_irq_lifecycle_var_t gpio_irq_lifecycle;
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    vsf_test_i2c_bus_scan_var_t i2c_bus_scan;
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    vsf_test_i2c_eeprom_page_var_t i2c_eeprom_page;
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    vsf_test_i2c_eeprom_rw_var_t i2c_eeprom_rw;
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
    vsf_test_i2c_eeprom_rw_fifo_var_t i2c_eeprom_rw_fifo;
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
    vsf_test_i2c_slave_var_t i2c_slave;
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
    vsf_test_i2c_slave_fifo_var_t i2c_slave_fifo;
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    vsf_test_rtc_alarm_var_t rtc_alarm;
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
    vsf_test_spi_async_var_t spi_async;
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    vsf_test_timer_async_var_t timer_async;
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    vsf_test_timer_oneshot_var_t timer_oneshot;
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    vsf_test_timer_periodic_var_t timer_periodic;
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_rx_irq_var_t usart_request_rx_irq;
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_tx_irq_var_t usart_request_tx_irq;
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_bulk_irq_var_t usart_rx_bulk_irq;
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    vsf_test_usart_rx_data_var_t usart_rx_data;
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_irq_var_t usart_rx_fifo_irq;
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_threshold_var_t usart_rx_fifo_threshold;
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_tx_fifo_irq_var_t usart_tx_fifo_irq;
#endif
} vsf_test_suites_t;

extern vsf_test_suites_t vsf_test_suites;
#endif // __VSF_TEST_SUITES_H__
