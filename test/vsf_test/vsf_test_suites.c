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

#include "vsf_test_suite_registry.h"

/*============================ SCENARIO STATE INSTANCE ========================*/

vsf_test_suites_t vsf_test_suites;

/*============================ STATIC DATA ==================================*/

static const vsf_test_all_params_t __vsf_test_params = {
#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
    .gpio_analog_mode = { VSF_TEST_GPIO_ANALOG_MODE_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
    .gpio_atomic = { VSF_TEST_GPIO_ATOMIC_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    .gpio_concurrent_prio = { VSF_TEST_GPIO_CONCURRENT_PRIO_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
    .gpio_direction = { VSF_TEST_GPIO_DIRECTION_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    .gpio_exti = { VSF_TEST_GPIO_EXTI_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
    .gpio_io_check = { VSF_TEST_GPIO_IO_CHECK_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    .gpio_irq_latency = { VSF_TEST_GPIO_IRQ_LATENCY_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    .gpio_irq_lifecycle = { VSF_TEST_GPIO_IRQ_LIFECYCLE_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
    .gpio_multi_pin = { VSF_TEST_GPIO_MULTI_PIN_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
    .gpio_open_drain = { VSF_TEST_GPIO_OPEN_DRAIN_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
    .gpio_output_input = { VSF_TEST_GPIO_OUTPUT_INPUT_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
    .gpio_pinmux = { VSF_TEST_GPIO_PINMUX_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
    .gpio_systimer_health = { VSF_TEST_GPIO_SYSTIMER_HEALTH_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
    .gpio_toggle = { VSF_TEST_GPIO_TOGGLE_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
    .gpio_toggle_freq = { VSF_TEST_GPIO_TOGGLE_FREQ_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
    .gpio_toggle_stress = { VSF_TEST_GPIO_TOGGLE_STRESS_PARAMS_INIT },
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
    .gpio_write_throughput = { VSF_TEST_GPIO_WRITE_THROUGHPUT_PARAMS_INIT },
#endif
#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
    .usart_request_cancel = { VSF_TEST_USART_REQUEST_CANCEL_PARAMS_INIT },
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    .usart_request_rx_irq = { VSF_TEST_USART_REQUEST_RX_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    .usart_request_tx_irq = { VSF_TEST_USART_REQUEST_TX_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    .usart_rx_baud = { VSF_TEST_USART_RX_BAUD_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
    .usart_rx_break_error = { VSF_TEST_USART_RX_BREAK_ERROR_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    .usart_rx_bulk_irq = { VSF_TEST_USART_RX_BULK_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    .usart_rx_data = { VSF_TEST_USART_RX_DATA_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    .usart_rx_fifo_irq = { VSF_TEST_USART_RX_FIFO_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    .usart_rx_fifo_threshold = { VSF_TEST_USART_RX_FIFO_THRESHOLD_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
    .usart_rx_frame_error = { VSF_TEST_USART_RX_FRAME_ERROR_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    .usart_rx_irq = { VSF_TEST_USART_RX_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    .usart_rx_mode = { VSF_TEST_USART_RX_MODE_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
    .usart_rx_overflow_error = { VSF_TEST_USART_RX_OVERFLOW_ERROR_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
    .usart_rx_parity_error = { VSF_TEST_USART_RX_PARITY_ERROR_PARAMS_INIT },
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    .usart_rx_timeout = { VSF_TEST_USART_RX_TIMEOUT_PARAMS_INIT },
#endif
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    .usart_baud = { VSF_TEST_USART_TX_BAUD_PARAMS_INIT },
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    .usart_tx_fifo_irq = { VSF_TEST_USART_TX_FIFO_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    .usart_mode = { VSF_TEST_USART_TX_MODE_PARAMS_INIT },
#endif
#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
    .usart_break_signal = { VSF_TEST_USART_BREAK_SIGNAL_PARAMS_INIT },
#endif
#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
    .usart_hw_flow_control = { VSF_TEST_USART_HW_FLOW_CONTROL_PARAMS_INIT },
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    .i2c_bus_scan = { VSF_TEST_I2C_BUS_SCAN_PARAMS_INIT },
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    .i2c_eeprom_page = { VSF_TEST_I2C_EEPROM_PAGE_PARAMS_INIT },
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    .i2c_eeprom_rw = { VSF_TEST_I2C_EEPROM_RW_PARAMS_INIT },
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
    .i2c_eeprom_rw_fifo = { VSF_TEST_I2C_EEPROM_RW_FIFO_PARAMS_INIT },
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
    .i2c_slave = { VSF_TEST_I2C_SLAVE_PARAMS_INIT },
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
    .i2c_slave_fifo = { VSF_TEST_I2C_SLAVE_FIFO_PARAMS_INIT },
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
    .spi0_async = { VSF_TEST_SPI_ASYNC_PARAMS_INIT },
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
    .spi0_loopback = { VSF_TEST_SPI_LOOPBACK_PARAMS_INIT },
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
    .rng0_basic = { VSF_TEST_RNG_BASIC_PARAMS_INIT },
#endif
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    .adc0_oneshot = { VSF_TEST_ADC_ONESHOT_PARAMS_INIT },
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    .adc0_stream = { VSF_TEST_ADC_STREAM_PARAMS_INIT },
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
    .adc0_temperature = { VSF_TEST_ADC_TEMPERATURE_PARAMS_INIT },
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    .pwm0_basic = { VSF_TEST_PWM_BASIC_PARAMS_INIT },
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    .pwm0_dual_channel = { VSF_TEST_PWM_DUAL_CHANNEL_PARAMS_INIT },
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
    .pwm0_irq = { VSF_TEST_PWM_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    .dma0_mem2mem = { VSF_TEST_DMA_MEM2MEM_PARAMS_INIT },
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    .dma0_mem2mem_irq = { VSF_TEST_DMA_MEM2MEM_IRQ_PARAMS_INIT },
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    .dma0_scatter_gather = { VSF_TEST_DMA_SCATTER_GATHER_PARAMS_INIT },
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    .timer0_async = { VSF_TEST_TIMER_ASYNC_PARAMS_INIT },
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    .timer0_oneshot = { VSF_TEST_TIMER_ONESHOT_PARAMS_INIT },
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    .timer0_periodic = { VSF_TEST_TIMER_PERIODIC_PARAMS_INIT },
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    .rtc0_alarm = { VSF_TEST_RTC_ALARM_PARAMS_INIT },
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
    .rtc0_epoch = { VSF_TEST_RTC_EPOCH_PARAMS_INIT },
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    .rtc0_set_get = { VSF_TEST_RTC_SET_GET_PARAMS_INIT },
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    .flash0_boundary = { VSF_TEST_FLASH_BOUNDARY_PARAMS_INIT },
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    .flash0_erase_program_read = { VSF_TEST_FLASH_ERASE_PROGRAM_READ_PARAMS_INIT },
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    .wdt0_basic = { VSF_TEST_WDT_BASIC_PARAMS_INIT },
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    .wdt0_reboot = { VSF_TEST_WDT_REBOOT_PARAMS_INIT },
#endif
};

static vsf_test_all_cases_t __all_cases = {
#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
    .gpio_analog_mode = { VSF_TEST_GPIO_ANALOG_MODE_CASES(__vsf_test_params.gpio_analog_mode, vsf_test_gpio_analog_mode_run, false) },
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
    .gpio_atomic = { VSF_TEST_GPIO_ATOMIC_CASES(__vsf_test_params.gpio_atomic, vsf_test_gpio_atomic_run, false) },
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
    .gpio_concurrent_prio = { VSF_TEST_GPIO_CONCURRENT_PRIO_CASES(__vsf_test_params.gpio_concurrent_prio, vsf_test_gpio_concurrent_prio_run, false) },
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
    .gpio_direction = { VSF_TEST_GPIO_DIRECTION_CASES(__vsf_test_params.gpio_direction, vsf_test_gpio_direction_run, false) },
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
    .gpio_exti = { VSF_TEST_GPIO_EXTI_CASES(__vsf_test_params.gpio_exti, vsf_test_gpio_exti_run, false) },
#endif
#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
    .gpio_io_check = { VSF_TEST_GPIO_IO_CHECK_CASES(__vsf_test_params.gpio_io_check, vsf_test_gpio_io_check_run, false) },
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
    .gpio_irq_latency = { VSF_TEST_GPIO_IRQ_LATENCY_CASES(__vsf_test_params.gpio_irq_latency, vsf_test_gpio_irq_latency_run, false) },
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
    .gpio_irq_lifecycle = { VSF_TEST_GPIO_IRQ_LIFECYCLE_CASES(__vsf_test_params.gpio_irq_lifecycle, vsf_test_gpio_irq_lifecycle_run, false) },
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
    .gpio_multi_pin = { VSF_TEST_GPIO_MULTI_PIN_CASES(__vsf_test_params.gpio_multi_pin, vsf_test_gpio_multi_pin_run, false) },
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
    .gpio_open_drain = { VSF_TEST_GPIO_OPEN_DRAIN_CASES(__vsf_test_params.gpio_open_drain, vsf_test_gpio_open_drain_run, false) },
#endif
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
    .gpio_output_input = { VSF_TEST_GPIO_OUTPUT_INPUT_CASES(__vsf_test_params.gpio_output_input, vsf_test_gpio_output_input_run, false) },
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
    .gpio_pinmux = { VSF_TEST_GPIO_PINMUX_CASES(__vsf_test_params.gpio_pinmux, vsf_test_gpio_pinmux_run, false) },
#endif
#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
    .gpio_systimer_health = { VSF_TEST_GPIO_SYSTIMER_HEALTH_CASES(__vsf_test_params.gpio_systimer_health, vsf_test_gpio_systimer_health_run, false) },
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
    .gpio_toggle = { VSF_TEST_GPIO_TOGGLE_CASES(__vsf_test_params.gpio_toggle, vsf_test_gpio_toggle_run, false) },
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
    .gpio_toggle_freq = { VSF_TEST_GPIO_TOGGLE_FREQ_CASES(__vsf_test_params.gpio_toggle_freq, vsf_test_gpio_toggle_freq_run, false) },
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
    .gpio_toggle_stress = { VSF_TEST_GPIO_TOGGLE_STRESS_CASES(__vsf_test_params.gpio_toggle_stress, vsf_test_gpio_toggle_stress_run, false) },
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
    .gpio_write_throughput = { VSF_TEST_GPIO_WRITE_THROUGHPUT_CASES(__vsf_test_params.gpio_write_throughput, vsf_test_gpio_write_throughput_run, false) },
#endif
#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
    .usart_request_cancel = { VSF_TEST_USART_REQUEST_CANCEL_CASES(__vsf_test_params.usart_request_cancel, vsf_test_usart_request_cancel_run, false) },
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    .usart_request_rx_irq = { VSF_TEST_USART_REQUEST_RX_IRQ_CASES(__vsf_test_params.usart_request_rx_irq, vsf_test_usart_request_rx_irq_run, true) },
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    .usart_request_tx_irq = { VSF_TEST_USART_REQUEST_TX_IRQ_CASES(__vsf_test_params.usart_request_tx_irq, vsf_test_usart_request_tx_irq_run, false) },
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    .usart_rx_baud = { VSF_TEST_USART_RX_BAUD_CASES(__vsf_test_params.usart_rx_baud, vsf_test_usart_rx_baud_run, true) },
#endif
#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
    .usart_rx_break_error = { VSF_TEST_USART_RX_BREAK_ERROR_CASES(__vsf_test_params.usart_rx_break_error, vsf_test_usart_rx_break_error_run, true) },
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    .usart_rx_bulk_irq = { VSF_TEST_USART_RX_BULK_IRQ_CASES(__vsf_test_params.usart_rx_bulk_irq, vsf_test_usart_rx_bulk_irq_run, true) },
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    .usart_rx_data = { VSF_TEST_USART_RX_DATA_CASES(__vsf_test_params.usart_rx_data, vsf_test_usart_rx_data_run, true) },
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    .usart_rx_fifo_irq = { VSF_TEST_USART_RX_FIFO_IRQ_CASES(__vsf_test_params.usart_rx_fifo_irq, vsf_test_usart_rx_fifo_irq_run, true) },
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    .usart_rx_fifo_threshold = { VSF_TEST_USART_RX_FIFO_THRESHOLD_CASES(__vsf_test_params.usart_rx_fifo_threshold, vsf_test_usart_rx_fifo_threshold_run, true) },
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
    .usart_rx_frame_error = { VSF_TEST_USART_RX_FRAME_ERROR_CASES(__vsf_test_params.usart_rx_frame_error, vsf_test_usart_rx_frame_error_run, true) },
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    .usart_rx_irq = { VSF_TEST_USART_RX_IRQ_CASES(__vsf_test_params.usart_rx_irq, vsf_test_usart_rx_irq_run, true) },
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    .usart_rx_mode = { VSF_TEST_USART_RX_MODE_CASES(__vsf_test_params.usart_rx_mode, vsf_test_usart_rx_mode_run, true) },
#endif
#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
    .usart_rx_overflow_error = { VSF_TEST_USART_RX_OVERFLOW_ERROR_CASES(__vsf_test_params.usart_rx_overflow_error, vsf_test_usart_rx_overflow_error_run, true) },
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
    .usart_rx_parity_error = { VSF_TEST_USART_RX_PARITY_ERROR_CASES(__vsf_test_params.usart_rx_parity_error, vsf_test_usart_rx_parity_error_run, true) },
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    .usart_rx_timeout = { VSF_TEST_USART_RX_TIMEOUT_CASES(__vsf_test_params.usart_rx_timeout, vsf_test_usart_rx_timeout_run, true) },
#endif
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    .usart_baud = { VSF_TEST_USART_TX_BAUD_CASES(__vsf_test_params.usart_baud, vsf_test_usart_baud_run, false) },
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    .usart_tx_fifo_irq = { VSF_TEST_USART_TX_FIFO_IRQ_CASES(__vsf_test_params.usart_tx_fifo_irq, vsf_test_usart_tx_fifo_irq_run, false) },
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    .usart_mode = { VSF_TEST_USART_TX_MODE_CASES(__vsf_test_params.usart_mode, vsf_test_usart_mode_run, false) },
#endif
#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
    .usart_break_signal = { VSF_TEST_USART_BREAK_SIGNAL_CASES(__vsf_test_params.usart_break_signal, vsf_test_usart_break_signal_run, false) },
#endif
#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
    .usart_hw_flow_control = { VSF_TEST_USART_HW_FLOW_CONTROL_CASES(__vsf_test_params.usart_hw_flow_control, vsf_test_usart_hw_flow_control_run, false) },
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
    .i2c_bus_scan = { VSF_TEST_I2C_BUS_SCAN_CASES(__vsf_test_params.i2c_bus_scan, vsf_test_i2c_bus_scan_run, false) },
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
    .i2c_eeprom_page = { VSF_TEST_I2C_EEPROM_PAGE_CASES(__vsf_test_params.i2c_eeprom_page, vsf_test_i2c_eeprom_page_run, false) },
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
    .i2c_eeprom_rw = { VSF_TEST_I2C_EEPROM_RW_CASES(__vsf_test_params.i2c_eeprom_rw, vsf_test_i2c_eeprom_rw_run, false) },
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
    .i2c_eeprom_rw_fifo = { VSF_TEST_I2C_EEPROM_RW_FIFO_CASES(__vsf_test_params.i2c_eeprom_rw_fifo, vsf_test_i2c_eeprom_rw_fifo_run, false) },
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
    .i2c_slave = { VSF_TEST_I2C_SLAVE_CASES(__vsf_test_params.i2c_slave, vsf_test_i2c_slave_run, false) },
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
    .i2c_slave_fifo = { VSF_TEST_I2C_SLAVE_FIFO_CASES(__vsf_test_params.i2c_slave_fifo, vsf_test_i2c_slave_fifo_run, false) },
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
    .spi0_async = { VSF_TEST_SPI_ASYNC_CASES(__vsf_test_params.spi0_async, vsf_test_spi_async_run, false) },
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
    .spi0_loopback = { VSF_TEST_SPI_LOOPBACK_CASES(__vsf_test_params.spi0_loopback, vsf_test_spi_loopback_run, false) },
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
    .rng0_basic = { VSF_TEST_RNG_BASIC_CASES(__vsf_test_params.rng0_basic, vsf_test_rng_basic_run, false) },
#endif
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    .adc0_oneshot = { VSF_TEST_ADC_ONESHOT_CASES(__vsf_test_params.adc0_oneshot, vsf_test_adc_oneshot_run, false) },
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    .adc0_stream = { VSF_TEST_ADC_STREAM_CASES(__vsf_test_params.adc0_stream, vsf_test_adc_stream_run, false) },
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
    .adc0_temperature = { VSF_TEST_ADC_TEMPERATURE_CASES(__vsf_test_params.adc0_temperature, vsf_test_adc_temperature_run, false) },
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
    .pwm0_basic = { VSF_TEST_PWM_BASIC_CASES(__vsf_test_params.pwm0_basic, vsf_test_pwm_basic_run, false) },
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
    .pwm0_dual_channel = { VSF_TEST_PWM_DUAL_CHANNEL_CASES(__vsf_test_params.pwm0_dual_channel, vsf_test_pwm_dual_channel_run, false) },
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
    .pwm0_irq = { VSF_TEST_PWM_IRQ_CASES(__vsf_test_params.pwm0_irq, vsf_test_pwm_irq_run, false) },
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
    .dma0_mem2mem = { VSF_TEST_DMA_MEM2MEM_CASES(__vsf_test_params.dma0_mem2mem, vsf_test_dma_mem2mem_run, false) },
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
    .dma0_mem2mem_irq = { VSF_TEST_DMA_MEM2MEM_IRQ_CASES(__vsf_test_params.dma0_mem2mem_irq, vsf_test_dma_mem2mem_irq_run, false) },
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
    .dma0_scatter_gather = { VSF_TEST_DMA_SCATTER_GATHER_CASES(__vsf_test_params.dma0_scatter_gather, vsf_test_dma_scatter_gather_run, false) },
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
    .timer0_async = { VSF_TEST_TIMER_ASYNC_CASES(__vsf_test_params.timer0_async, vsf_test_timer_async_run, false) },
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    .timer0_oneshot = { VSF_TEST_TIMER_ONESHOT_CASES(__vsf_test_params.timer0_oneshot, vsf_test_timer_oneshot_run, false) },
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    .timer0_periodic = { VSF_TEST_TIMER_PERIODIC_CASES(__vsf_test_params.timer0_periodic, vsf_test_timer_periodic_run, false) },
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    .rtc0_alarm = { VSF_TEST_RTC_ALARM_CASES(__vsf_test_params.rtc0_alarm, vsf_test_rtc_alarm_run, false) },
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
    .rtc0_epoch = { VSF_TEST_RTC_EPOCH_CASES(__vsf_test_params.rtc0_epoch, vsf_test_rtc_epoch_run, false) },
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    .rtc0_set_get = { VSF_TEST_RTC_SET_GET_CASES(__vsf_test_params.rtc0_set_get, vsf_test_rtc_set_get_run, false) },
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
    .flash0_boundary = { VSF_TEST_FLASH_BOUNDARY_CASES(__vsf_test_params.flash0_boundary, vsf_test_flash_boundary_run, false) },
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
    .flash0_erase_program_read = { VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASES(__vsf_test_params.flash0_erase_program_read, vsf_test_flash_erase_program_read_run, false) },
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
    .wdt0_basic = { VSF_TEST_WDT_BASIC_CASES(__vsf_test_params.wdt0_basic, vsf_test_wdt_basic_run, false) },
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
    .wdt0_reboot = { VSF_TEST_WDT_REBOOT_CASES(__vsf_test_params.wdt0_reboot, vsf_test_wdt_reboot_run, false) },
#endif
};

/*============================ SUITES ========================================*/

const vsf_test_suite_t __vsf_test_suites[] = {
#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
        {
            .name       = "gpio_analog_mode",
            .cases      = __all_cases.gpio_analog_mode,
            .case_count = VSF_TEST_GPIO_ANALOG_MODE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
        {
            .name       = "gpio_atomic",
            .cases      = __all_cases.gpio_atomic,
            .case_count = VSF_TEST_GPIO_ATOMIC_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
        {
            .name       = "gpio_concurrent_prio",
            .cases      = __all_cases.gpio_concurrent_prio,
            .case_count = VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
        {
            .name       = "gpio_direction",
            .cases      = __all_cases.gpio_direction,
            .case_count = VSF_TEST_GPIO_DIRECTION_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
        {
            .name       = "gpio_exti",
            .cases      = __all_cases.gpio_exti,
            .case_count = VSF_TEST_GPIO_EXTI_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
        {
            .name       = "gpio_io_check",
            .cases      = __all_cases.gpio_io_check,
            .case_count = VSF_TEST_GPIO_IO_CHECK_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
        {
            .name       = "gpio_irq_latency",
            .cases      = __all_cases.gpio_irq_latency,
            .case_count = VSF_TEST_GPIO_IRQ_LATENCY_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
        {
            .name       = "gpio_irq_lifecycle",
            .cases      = __all_cases.gpio_irq_lifecycle,
            .case_count = VSF_TEST_GPIO_IRQ_LIFECYCLE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
        {
            .name       = "gpio_multi_pin",
            .cases      = __all_cases.gpio_multi_pin,
            .case_count = VSF_TEST_GPIO_MULTI_PIN_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
        {
            .name       = "gpio_open_drain",
            .cases      = __all_cases.gpio_open_drain,
            .case_count = VSF_TEST_GPIO_OPEN_DRAIN_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
        {
            .name       = "gpio_output_input",
            .cases      = __all_cases.gpio_output_input,
            .case_count = VSF_TEST_GPIO_OUTPUT_INPUT_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
        {
            .name       = "gpio_pinmux",
            .cases      = __all_cases.gpio_pinmux,
            .case_count = VSF_TEST_GPIO_PINMUX_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_NONE,
        },
#endif
#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
        {
            .name       = "gpio_systimer_health",
            .cases      = __all_cases.gpio_systimer_health,
            .case_count = VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_TOGGLE_ENABLE == ENABLED
        {
            .name       = "gpio_toggle",
            .cases      = __all_cases.gpio_toggle,
            .case_count = VSF_TEST_GPIO_TOGGLE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
        {
            .name       = "gpio_toggle_freq",
            .cases      = __all_cases.gpio_toggle_freq,
            .case_count = VSF_TEST_GPIO_TOGGLE_FREQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
        {
            .name       = "gpio_toggle_stress",
            .cases      = __all_cases.gpio_toggle_stress,
            .case_count = VSF_TEST_GPIO_TOGGLE_STRESS_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
        {
            .name       = "gpio_write_throughput",
            .cases      = __all_cases.gpio_write_throughput,
            .case_count = VSF_TEST_GPIO_WRITE_THROUGHPUT_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        },
#endif
#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
        {
            .name       = "usart_request_cancel",
            .cases      = __all_cases.usart_request_cancel,
            .case_count = VSF_TEST_USART_REQUEST_CANCEL_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
        {
            .name       = "usart_request_rx_irq",
            .cases      = __all_cases.usart_request_rx_irq,
            .case_count = VSF_TEST_USART_REQUEST_RX_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
        {
            .name       = "usart_request_tx_irq",
            .cases      = __all_cases.usart_request_tx_irq,
            .case_count = VSF_TEST_USART_REQUEST_TX_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
        {
            .name       = "usart_rx_baud",
            .cases      = __all_cases.usart_rx_baud,
            .case_count = VSF_TEST_USART_RX_BAUD_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
        {
            .name       = "usart_rx_break_error",
            .cases      = __all_cases.usart_rx_break_error,
            .case_count = VSF_TEST_USART_RX_BREAK_ERROR_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
        {
            .name       = "usart_rx_bulk_irq",
            .cases      = __all_cases.usart_rx_bulk_irq,
            .case_count = VSF_TEST_USART_RX_BULK_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
        {
            .name       = "usart_rx_data",
            .cases      = __all_cases.usart_rx_data,
            .case_count = VSF_TEST_USART_RX_DATA_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
        {
            .name       = "usart_rx_fifo_irq",
            .cases      = __all_cases.usart_rx_fifo_irq,
            .case_count = VSF_TEST_USART_RX_FIFO_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
        {
            .name       = "usart_rx_fifo_threshold",
            .cases      = __all_cases.usart_rx_fifo_threshold,
            .case_count = VSF_TEST_USART_RX_FIFO_THRESHOLD_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
        {
            .name       = "usart_rx_frame_error",
            .cases      = __all_cases.usart_rx_frame_error,
            .case_count = VSF_TEST_USART_RX_FRAME_ERROR_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
        {
            .name       = "usart_rx_irq",
            .cases      = __all_cases.usart_rx_irq,
            .case_count = VSF_TEST_USART_RX_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
        {
            .name       = "usart_rx_mode",
            .cases      = __all_cases.usart_rx_mode,
            .case_count = VSF_TEST_USART_RX_MODE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
        {
            .name       = "usart_rx_overflow_error",
            .cases      = __all_cases.usart_rx_overflow_error,
            .case_count = VSF_TEST_USART_RX_OVERFLOW_ERROR_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
        {
            .name       = "usart_rx_parity_error",
            .cases      = __all_cases.usart_rx_parity_error,
            .case_count = VSF_TEST_USART_RX_PARITY_ERROR_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
        {
            .name       = "usart_rx_timeout",
            .cases      = __all_cases.usart_rx_timeout,
            .case_count = VSF_TEST_USART_RX_TIMEOUT_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
        {
            .name       = "usart_baud",
            .cases      = __all_cases.usart_baud,
            .case_count = VSF_TEST_USART_TX_BAUD_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
        {
            .name       = "usart_tx_fifo_irq",
            .cases      = __all_cases.usart_tx_fifo_irq,
            .case_count = VSF_TEST_USART_TX_FIFO_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
        {
            .name       = "usart_mode",
            .cases      = __all_cases.usart_mode,
            .case_count = VSF_TEST_USART_TX_MODE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
        {
            .name       = "usart_break_signal",
            .cases      = __all_cases.usart_break_signal,
            .case_count = VSF_TEST_USART_BREAK_SIGNAL_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
        {
            .name       = "usart_hw_flow_control",
            .cases      = __all_cases.usart_hw_flow_control,
            .case_count = VSF_TEST_USART_HW_FLOW_CONTROL_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        },
#endif
#if VSF_TEST_I2C_BUS_SCAN_ENABLE == ENABLED
        {
            .name       = "i2c_bus_scan",
            .cases      = __all_cases.i2c_bus_scan,
            .case_count = VSF_TEST_I2C_BUS_SCAN_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C,
        },
#endif
#if VSF_TEST_I2C_EEPROM_PAGE_ENABLE == ENABLED
        {
            .name       = "i2c_eeprom_page",
            .cases      = __all_cases.i2c_eeprom_page,
            .case_count = VSF_TEST_I2C_EEPROM_PAGE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C,
        },
#endif
#if VSF_TEST_I2C_EEPROM_RW_ENABLE == ENABLED
        {
            .name       = "i2c_eeprom_rw",
            .cases      = __all_cases.i2c_eeprom_rw,
            .case_count = VSF_TEST_I2C_EEPROM_RW_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C,
        },
#endif
#if VSF_TEST_I2C_EEPROM_RW_FIFO_ENABLE == ENABLED
        {
            .name       = "i2c_eeprom_rw_fifo",
            .cases      = __all_cases.i2c_eeprom_rw_fifo,
            .case_count = VSF_TEST_I2C_EEPROM_RW_FIFO_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_I2C,
        },
#endif
#if VSF_TEST_I2C_SLAVE_ENABLE == ENABLED
        {
            .name       = "i2c_slave",
            .cases      = __all_cases.i2c_slave,
            .case_count = VSF_TEST_I2C_SLAVE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_NONE,
        },
#endif
#if VSF_TEST_I2C_SLAVE_FIFO_ENABLE == ENABLED
        {
            .name       = "i2c_slave_fifo",
            .cases      = __all_cases.i2c_slave_fifo,
            .case_count = VSF_TEST_I2C_SLAVE_FIFO_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_NONE,
        },
#endif
#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
        {
            .name       = "spi_async",
            .cases      = __all_cases.spi0_async,
            .case_count = VSF_TEST_SPI_ASYNC_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_SPI,
        },
#endif
#if VSF_TEST_SPI_LOOPBACK_ENABLE == ENABLED
        {
            .name       = "spi_loopback",
            .cases      = __all_cases.spi0_loopback,
            .case_count = VSF_TEST_SPI_LOOPBACK_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_SPI,
        },
#endif
#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
        {
            .name       = "rng_basic",
            .cases      = __all_cases.rng0_basic,
            .case_count = VSF_TEST_RNG_BASIC_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_RNG,
        },
#endif
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
        {
            .name       = "adc_oneshot",
            .cases      = __all_cases.adc0_oneshot,
            .case_count = VSF_TEST_ADC_ONESHOT_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_ADC,
        },
#endif
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
        {
            .name       = "adc_stream",
            .cases      = __all_cases.adc0_stream,
            .case_count = VSF_TEST_ADC_STREAM_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_ADC,
        },
#endif
#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
        {
            .name       = "adc_temperature",
            .cases      = __all_cases.adc0_temperature,
            .case_count = VSF_TEST_ADC_TEMPERATURE_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_ADC,
        },
#endif
#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
        {
            .name       = "pwm_basic",
            .cases      = __all_cases.pwm0_basic,
            .case_count = VSF_TEST_PWM_BASIC_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_PWM,
        },
#endif
#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
        {
            .name       = "pwm_dual_channel",
            .cases      = __all_cases.pwm0_dual_channel,
            .case_count = VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_PWM,
        },
#endif
#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
        {
            .name       = "pwm_irq",
            .cases      = __all_cases.pwm0_irq,
            .case_count = VSF_TEST_PWM_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_PWM,
        },
#endif
#if VSF_TEST_DMA_MEM2MEM_ENABLE == ENABLED
        {
            .name       = "dma_mem2mem",
            .cases      = __all_cases.dma0_mem2mem,
            .case_count = VSF_TEST_DMA_MEM2MEM_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_DMA,
        },
#endif
#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
        {
            .name       = "dma_mem2mem_irq",
            .cases      = __all_cases.dma0_mem2mem_irq,
            .case_count = VSF_TEST_DMA_MEM2MEM_IRQ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_DMA,
        },
#endif
#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
        {
            .name       = "dma_scatter_gather",
            .cases      = __all_cases.dma0_scatter_gather,
            .case_count = VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_DMA,
        },
#endif
#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
        {
            .name       = "timer_async",
            .cases      = __all_cases.timer0_async,
            .case_count = VSF_TEST_TIMER_ASYNC_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER,
        },
#endif
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
        {
            .name       = "timer_oneshot",
            .cases      = __all_cases.timer0_oneshot,
            .case_count = VSF_TEST_TIMER_ONESHOT_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER,
        },
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
        {
            .name       = "timer_periodic",
            .cases      = __all_cases.timer0_periodic,
            .case_count = VSF_TEST_TIMER_PERIODIC_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER,
        },
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
        {
            .name       = "rtc_alarm",
            .cases      = __all_cases.rtc0_alarm,
            .case_count = VSF_TEST_RTC_ALARM_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_RTC,
        },
#endif
#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
        {
            .name       = "rtc_epoch",
            .cases      = __all_cases.rtc0_epoch,
            .case_count = VSF_TEST_RTC_EPOCH_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_RTC,
        },
#endif
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
        {
            .name       = "rtc_set_get",
            .cases      = __all_cases.rtc0_set_get,
            .case_count = VSF_TEST_RTC_SET_GET_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_RTC,
        },
#endif
#if VSF_TEST_FLASH_BOUNDARY_ENABLE == ENABLED
        {
            .name       = "flash_boundary",
            .cases      = __all_cases.flash0_boundary,
            .case_count = VSF_TEST_FLASH_BOUNDARY_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_FLASH,
        },
#endif
#if VSF_TEST_FLASH_ERASE_PROGRAM_READ_ENABLE == ENABLED
        {
            .name       = "flash_erase_program_read",
            .cases      = __all_cases.flash0_erase_program_read,
            .case_count = VSF_TEST_FLASH_ERASE_PROGRAM_READ_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_FLASH,
        },
#endif
#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
        {
            .name       = "wdt_basic",
            .cases      = __all_cases.wdt0_basic,
            .case_count = VSF_TEST_WDT_BASIC_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_WDT,
        },
#endif
#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
        {
            .name       = "wdt_reboot",
            .cases      = __all_cases.wdt0_reboot,
            .case_count = VSF_TEST_WDT_REBOOT_CASE_COUNT,
            .peripheral_type = VSF_PERIPHERAL_TYPE_WDT,
        },
#endif
};
uint8_t __vsf_test_suite_count = dimof(__vsf_test_suites);

/*============================ REGISTRY ======================================*/

/*============================ REGISTRY ======================================*/
