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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#pragma once

#include "vsf.h"
#include "vsf_board.h"
#include "component/test/vsf_test/vsf_test.h"
#include "vsf_test_suites.h"

/*============================ LOCAL FUNCTIONS ===============================*/



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
    vsf_test_case_t spi0_async[VSF_TEST_SPI_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
    vsf_test_case_t spi0_loopback[VSF_TEST_SPI_LOOPBACK_CASE_COUNT];
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
    vsf_test_case_t rng0_basic[VSF_TEST_RNG_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    vsf_test_case_t adc0_oneshot[VSF_TEST_ADC_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    vsf_test_case_t adc0_stream[VSF_TEST_ADC_STREAM_CASE_COUNT];
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
    vsf_test_case_t adc0_temperature[VSF_TEST_ADC_TEMPERATURE_CASE_COUNT];
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    vsf_test_case_t pwm0_basic[VSF_TEST_PWM_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    vsf_test_case_t pwm0_dual_channel[VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT];
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
    vsf_test_case_t pwm0_irq[VSF_TEST_PWM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    vsf_test_case_t dma0_mem2mem[VSF_TEST_DMA_MEM2MEM_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    vsf_test_case_t dma0_mem2mem_irq[VSF_TEST_DMA_MEM2MEM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    vsf_test_case_t dma0_scatter_gather[VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    vsf_test_case_t timer0_async[VSF_TEST_TIMER_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    vsf_test_case_t timer0_oneshot[VSF_TEST_TIMER_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    vsf_test_case_t timer0_periodic[VSF_TEST_TIMER_PERIODIC_CASE_COUNT];
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    vsf_test_case_t rtc0_alarm[VSF_TEST_RTC_ALARM_CASE_COUNT];
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
    vsf_test_case_t rtc0_epoch[VSF_TEST_RTC_EPOCH_CASE_COUNT];
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    vsf_test_case_t rtc0_set_get[VSF_TEST_RTC_SET_GET_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    vsf_test_case_t flash0_boundary[VSF_TEST_FLASH_BOUNDARY_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    vsf_test_case_t flash0_erase_program_read[VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASE_COUNT];
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    vsf_test_case_t wdt0_basic[VSF_TEST_WDT_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    vsf_test_case_t wdt0_reboot[VSF_TEST_WDT_REBOOT_CASE_COUNT];
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
    vsf_test_spi_async_params_t spi0_async[VSF_TEST_SPI_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
    vsf_test_spi_loopback_params_t spi0_loopback[VSF_TEST_SPI_LOOPBACK_CASE_COUNT];
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
    vsf_test_rng_basic_params_t rng0_basic[VSF_TEST_RNG_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    vsf_test_adc_oneshot_params_t adc0_oneshot[VSF_TEST_ADC_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    vsf_test_adc_stream_params_t adc0_stream[VSF_TEST_ADC_STREAM_CASE_COUNT];
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
    vsf_test_adc_temperature_params_t adc0_temperature[VSF_TEST_ADC_TEMPERATURE_CASE_COUNT];
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    vsf_test_pwm_basic_params_t pwm0_basic[VSF_TEST_PWM_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    vsf_test_pwm_dual_channel_params_t pwm0_dual_channel[VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT];
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
    vsf_test_pwm_irq_params_t pwm0_irq[VSF_TEST_PWM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    vsf_test_dma_mem2mem_params_t dma0_mem2mem[VSF_TEST_DMA_MEM2MEM_CASE_COUNT];
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    vsf_test_dma_mem2mem_irq_params_t dma0_mem2mem_irq[VSF_TEST_DMA_MEM2MEM_IRQ_CASE_COUNT];
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    vsf_test_dma_scatter_gather_params_t dma0_scatter_gather[VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    vsf_test_timer_async_params_t timer0_async[VSF_TEST_TIMER_ASYNC_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    vsf_test_timer_oneshot_params_t timer0_oneshot[VSF_TEST_TIMER_ONESHOT_CASE_COUNT];
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    vsf_test_timer_periodic_params_t timer0_periodic[VSF_TEST_TIMER_PERIODIC_CASE_COUNT];
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    vsf_test_rtc_alarm_params_t rtc0_alarm[VSF_TEST_RTC_ALARM_CASE_COUNT];
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
    vsf_test_rtc_epoch_params_t rtc0_epoch[VSF_TEST_RTC_EPOCH_CASE_COUNT];
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    vsf_test_rtc_set_get_params_t rtc0_set_get[VSF_TEST_RTC_SET_GET_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    vsf_test_flash_boundary_params_t flash0_boundary[VSF_TEST_FLASH_BOUNDARY_CASE_COUNT];
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    vsf_test_flash_erase_program_read_params_t flash0_erase_program_read[VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASE_COUNT];
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    vsf_test_wdt_basic_params_t wdt0_basic[VSF_TEST_WDT_BASIC_CASE_COUNT];
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    vsf_test_wdt_reboot_params_t wdt0_reboot[VSF_TEST_WDT_REBOOT_CASE_COUNT];
#endif
} vsf_test_all_params_t;

/*============================ PUBLIC VARIABLES ==============================*/

extern const vsf_test_inst_t __board_test_instances[];
extern uint8_t __board_test_instance_count;
extern const vsf_test_suite_t __vsf_test_suites[];
extern uint8_t __vsf_test_suite_count;
