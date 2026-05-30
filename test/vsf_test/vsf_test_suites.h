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

/*============================ AGGREGATED DATA TYPES ========================*/

typedef struct {
#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
    vsf_test_case_t gpio_analog_mode[VSF_TEST_GPIO_ANALOG_MODE_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
    vsf_test_case_t gpio_atomic[VSF_TEST_GPIO_ATOMIC_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    vsf_test_case_t gpio_concurrent_prio[VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
    vsf_test_case_t gpio_direction[VSF_TEST_GPIO_DIRECTION_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    vsf_test_case_t gpio_exti[VSF_TEST_GPIO_EXTI_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
    vsf_test_case_t gpio_io_check[VSF_TEST_GPIO_IO_CHECK_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    vsf_test_case_t gpio_irq_latency[VSF_TEST_GPIO_IRQ_LATENCY_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    vsf_test_case_t gpio_irq_lifecycle[VSF_TEST_GPIO_IRQ_LIFECYCLE_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
    vsf_test_case_t gpio_multi_pin[VSF_TEST_GPIO_MULTI_PIN_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
    vsf_test_case_t gpio_open_drain[VSF_TEST_GPIO_OPEN_DRAIN_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
    vsf_test_case_t gpio_output_input[VSF_TEST_GPIO_OUTPUT_INPUT_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
    vsf_test_case_t gpio_pinmux[VSF_TEST_GPIO_PINMUX_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
    vsf_test_case_t gpio_systimer_health[VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
    vsf_test_case_t gpio_toggle[VSF_TEST_GPIO_TOGGLE_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
    vsf_test_case_t gpio_toggle_freq[VSF_TEST_GPIO_TOGGLE_FREQ_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
    vsf_test_case_t gpio_toggle_stress[VSF_TEST_GPIO_TOGGLE_STRESS_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
    vsf_test_case_t gpio_write_throughput[VSF_TEST_GPIO_WRITE_THROUGHPUT_CASE_COUNT];
#endif
#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
    vsf_test_case_t usart_request_cancel[VSF_TEST_USART_REQUEST_CANCEL_CASE_COUNT];
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    vsf_test_case_t usart_request_rx_irq[VSF_TEST_USART_REQUEST_RX_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    vsf_test_case_t usart_request_tx_irq[VSF_TEST_USART_REQUEST_TX_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    vsf_test_case_t usart_rx_baud[VSF_TEST_USART_RX_BAUD_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
    vsf_test_case_t usart_rx_break_error[VSF_TEST_USART_RX_BREAK_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    vsf_test_case_t usart_rx_bulk_irq[VSF_TEST_USART_RX_BULK_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    vsf_test_case_t usart_rx_data[VSF_TEST_USART_RX_DATA_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_case_t usart_rx_fifo_irq[VSF_TEST_USART_RX_FIFO_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    vsf_test_case_t usart_rx_fifo_threshold[VSF_TEST_USART_RX_FIFO_THRESHOLD_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
    vsf_test_case_t usart_rx_frame_error[VSF_TEST_USART_RX_FRAME_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    vsf_test_case_t usart_rx_irq[VSF_TEST_USART_RX_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    vsf_test_case_t usart_rx_mode[VSF_TEST_USART_RX_MODE_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
    vsf_test_case_t usart_rx_overflow_error[VSF_TEST_USART_RX_OVERFLOW_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
    vsf_test_case_t usart_rx_parity_error[VSF_TEST_USART_RX_PARITY_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    vsf_test_case_t usart_rx_timeout[VSF_TEST_USART_RX_TIMEOUT_CASE_COUNT];
#endif
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    vsf_test_case_t usart_baud[VSF_TEST_USART_TX_BAUD_CASE_COUNT];
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_case_t usart_tx_fifo_irq[VSF_TEST_USART_TX_FIFO_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    vsf_test_case_t usart_mode[VSF_TEST_USART_TX_MODE_CASE_COUNT];
#endif
#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
    vsf_test_case_t usart_break_signal[VSF_TEST_USART_BREAK_SIGNAL_CASE_COUNT];
#endif
#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
    vsf_test_case_t usart_hw_flow_control[VSF_TEST_USART_HW_FLOW_CONTROL_CASE_COUNT];
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    vsf_test_case_t i2c_bus_scan[VSF_TEST_I2C_BUS_SCAN_CASE_COUNT];
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    vsf_test_case_t i2c_eeprom_page[VSF_TEST_I2C_EEPROM_PAGE_CASE_COUNT];
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    vsf_test_case_t i2c_eeprom_rw[VSF_TEST_I2C_EEPROM_RW_CASE_COUNT];
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
    vsf_test_case_t i2c_eeprom_rw_fifo[VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_COUNT];
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
    vsf_test_case_t i2c_slave[VSF_TEST_I2C_SLAVE_CASE_COUNT];
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
    vsf_test_case_t i2c_slave_fifo[VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT];
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
    vsf_test_case_t spi_async[VSF_TEST_SPI_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
    vsf_test_case_t spi_loopback[VSF_TEST_SPI_LOOPBACK_CASE_COUNT];
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
    vsf_test_case_t rng_basic[VSF_TEST_RNG_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    vsf_test_case_t adc_oneshot[VSF_TEST_ADC_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    vsf_test_case_t adc_stream[VSF_TEST_ADC_STREAM_CASE_COUNT];
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
    vsf_test_case_t adc_temperature[VSF_TEST_ADC_TEMPERATURE_CASE_COUNT];
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    vsf_test_case_t pwm_basic[VSF_TEST_PWM_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    vsf_test_case_t pwm_dual_channel[VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT];
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
    vsf_test_case_t pwm_irq[VSF_TEST_PWM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    vsf_test_case_t dma_mem2mem[VSF_TEST_DMA_MEM2MEM_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    vsf_test_case_t dma_mem2mem_irq[VSF_TEST_DMA_MEM2MEM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    vsf_test_case_t dma_scatter_gather[VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    vsf_test_case_t timer_async[VSF_TEST_TIMER_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    vsf_test_case_t timer_oneshot[VSF_TEST_TIMER_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    vsf_test_case_t timer_periodic[VSF_TEST_TIMER_PERIODIC_CASE_COUNT];
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    vsf_test_case_t rtc_alarm[VSF_TEST_RTC_ALARM_CASE_COUNT];
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
    vsf_test_case_t rtc_epoch[VSF_TEST_RTC_EPOCH_CASE_COUNT];
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    vsf_test_case_t rtc_set_get[VSF_TEST_RTC_SET_GET_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    vsf_test_case_t flash_boundary[VSF_TEST_FLASH_BOUNDARY_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    vsf_test_case_t flash_erase_program_read[VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASE_COUNT];
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    vsf_test_case_t wdt_basic[VSF_TEST_WDT_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    vsf_test_case_t wdt_reboot[VSF_TEST_WDT_REBOOT_CASE_COUNT];
#endif
} vsf_test_all_cases_t;

typedef struct {
#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
    vsf_test_gpio_analog_mode_params_t gpio_analog_mode[VSF_TEST_GPIO_ANALOG_MODE_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
    vsf_test_gpio_atomic_params_t gpio_atomic[VSF_TEST_GPIO_ATOMIC_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    vsf_test_gpio_concurrent_prio_params_t gpio_concurrent_prio[VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
    vsf_test_gpio_direction_params_t gpio_direction[VSF_TEST_GPIO_DIRECTION_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    vsf_test_gpio_exti_params_t gpio_exti[VSF_TEST_GPIO_EXTI_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
    vsf_test_gpio_io_check_params_t gpio_io_check[VSF_TEST_GPIO_IO_CHECK_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    vsf_test_gpio_irq_latency_params_t gpio_irq_latency[VSF_TEST_GPIO_IRQ_LATENCY_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    vsf_test_gpio_irq_lifecycle_params_t gpio_irq_lifecycle[VSF_TEST_GPIO_IRQ_LIFECYCLE_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
    vsf_test_gpio_multi_pin_params_t gpio_multi_pin[VSF_TEST_GPIO_MULTI_PIN_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
    vsf_test_gpio_open_drain_params_t gpio_open_drain[VSF_TEST_GPIO_OPEN_DRAIN_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
    vsf_test_gpio_output_input_params_t gpio_output_input[VSF_TEST_GPIO_OUTPUT_INPUT_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
    vsf_test_gpio_pinmux_params_t gpio_pinmux[VSF_TEST_GPIO_PINMUX_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
    vsf_test_gpio_systimer_health_params_t gpio_systimer_health[VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
    vsf_test_gpio_toggle_params_t gpio_toggle[VSF_TEST_GPIO_TOGGLE_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
    vsf_test_gpio_toggle_freq_params_t gpio_toggle_freq[VSF_TEST_GPIO_TOGGLE_FREQ_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
    vsf_test_gpio_toggle_stress_params_t gpio_toggle_stress[VSF_TEST_GPIO_TOGGLE_STRESS_CASE_COUNT];
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
    vsf_test_gpio_write_throughput_params_t gpio_write_throughput[VSF_TEST_GPIO_WRITE_THROUGHPUT_CASE_COUNT];
#endif
#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
    vsf_test_usart_request_cancel_params_t usart_request_cancel[VSF_TEST_USART_REQUEST_CANCEL_CASE_COUNT];
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_rx_irq_params_t usart_request_rx_irq[VSF_TEST_USART_REQUEST_RX_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_tx_irq_params_t usart_request_tx_irq[VSF_TEST_USART_REQUEST_TX_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    vsf_test_usart_rx_baud_params_t usart_rx_baud[VSF_TEST_USART_RX_BAUD_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_break_error_params_t usart_rx_break_error[VSF_TEST_USART_RX_BREAK_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_bulk_irq_params_t usart_rx_bulk_irq[VSF_TEST_USART_RX_BULK_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    vsf_test_usart_rx_data_params_t usart_rx_data[VSF_TEST_USART_RX_DATA_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_irq_params_t usart_rx_fifo_irq[VSF_TEST_USART_RX_FIFO_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_threshold_params_t usart_rx_fifo_threshold[VSF_TEST_USART_RX_FIFO_THRESHOLD_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_frame_error_params_t usart_rx_frame_error[VSF_TEST_USART_RX_FRAME_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_irq_params_t usart_rx_irq[VSF_TEST_USART_RX_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    vsf_test_usart_rx_mode_params_t usart_rx_mode[VSF_TEST_USART_RX_MODE_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_overflow_error_params_t usart_rx_overflow_error[VSF_TEST_USART_RX_OVERFLOW_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_parity_error_params_t usart_rx_parity_error[VSF_TEST_USART_RX_PARITY_ERROR_CASE_COUNT];
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    vsf_test_usart_rx_timeout_params_t usart_rx_timeout[VSF_TEST_USART_RX_TIMEOUT_CASE_COUNT];
#endif
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    vsf_test_usart_baud_params_t usart_baud[VSF_TEST_USART_TX_BAUD_CASE_COUNT];
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_tx_fifo_irq_params_t usart_tx_fifo_irq[VSF_TEST_USART_TX_FIFO_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    vsf_test_usart_mode_params_t usart_mode[VSF_TEST_USART_TX_MODE_CASE_COUNT];
#endif
#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
    vsf_test_usart_break_signal_params_t usart_break_signal[VSF_TEST_USART_BREAK_SIGNAL_CASE_COUNT];
#endif
#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
    vsf_test_usart_hw_flow_control_params_t usart_hw_flow_control[VSF_TEST_USART_HW_FLOW_CONTROL_CASE_COUNT];
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    vsf_test_i2c_bus_scan_params_t i2c_bus_scan[VSF_TEST_I2C_BUS_SCAN_CASE_COUNT];
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    vsf_test_i2c_eeprom_page_params_t i2c_eeprom_page[VSF_TEST_I2C_EEPROM_PAGE_CASE_COUNT];
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    vsf_test_i2c_eeprom_rw_params_t i2c_eeprom_rw[VSF_TEST_I2C_EEPROM_RW_CASE_COUNT];
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
    vsf_test_i2c_eeprom_rw_fifo_params_t i2c_eeprom_rw_fifo[VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_COUNT];
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
    vsf_test_i2c_slave_params_t i2c_slave[VSF_TEST_I2C_SLAVE_CASE_COUNT];
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
    vsf_test_i2c_slave_fifo_params_t i2c_slave_fifo[VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT];
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
    vsf_test_spi_async_params_t spi_async[VSF_TEST_SPI_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
    vsf_test_spi_loopback_params_t spi_loopback[VSF_TEST_SPI_LOOPBACK_CASE_COUNT];
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
    vsf_test_rng_basic_params_t rng_basic[VSF_TEST_RNG_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    vsf_test_adc_oneshot_params_t adc_oneshot[VSF_TEST_ADC_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    vsf_test_adc_stream_params_t adc_stream[VSF_TEST_ADC_STREAM_CASE_COUNT];
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
    vsf_test_adc_temperature_params_t adc_temperature[VSF_TEST_ADC_TEMPERATURE_CASE_COUNT];
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    vsf_test_pwm_basic_params_t pwm_basic[VSF_TEST_PWM_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    vsf_test_pwm_dual_channel_params_t pwm_dual_channel[VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT];
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
    vsf_test_pwm_irq_params_t pwm_irq[VSF_TEST_PWM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    vsf_test_dma_mem2mem_params_t dma_mem2mem[VSF_TEST_DMA_MEM2MEM_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    vsf_test_dma_mem2mem_irq_params_t dma_mem2mem_irq[VSF_TEST_DMA_MEM2MEM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    vsf_test_dma_scatter_gather_params_t dma_scatter_gather[VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    vsf_test_timer_async_params_t timer_async[VSF_TEST_TIMER_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    vsf_test_timer_oneshot_params_t timer_oneshot[VSF_TEST_TIMER_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    vsf_test_timer_periodic_params_t timer_periodic[VSF_TEST_TIMER_PERIODIC_CASE_COUNT];
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    vsf_test_rtc_alarm_params_t rtc_alarm[VSF_TEST_RTC_ALARM_CASE_COUNT];
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
    vsf_test_rtc_epoch_params_t rtc_epoch[VSF_TEST_RTC_EPOCH_CASE_COUNT];
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    vsf_test_rtc_set_get_params_t rtc_set_get[VSF_TEST_RTC_SET_GET_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    vsf_test_flash_boundary_params_t flash_boundary[VSF_TEST_FLASH_BOUNDARY_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    vsf_test_flash_erase_program_read_params_t flash_erase_program_read[VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASE_COUNT];
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    vsf_test_wdt_basic_params_t wdt_basic[VSF_TEST_WDT_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    vsf_test_wdt_reboot_params_t wdt_reboot[VSF_TEST_WDT_REBOOT_CASE_COUNT];
#endif
} vsf_test_all_params_t;

/*============================ PUBLIC VARIABLES ==============================*/

extern const vsf_test_inst_t __board_test_instances[];
extern uint8_t __board_test_instance_count;
extern const vsf_test_suite_t vsf_test_suite_list[];
extern uint8_t vsf_test_suite_count;

#endif // __VSF_TEST_SUITES_H__
