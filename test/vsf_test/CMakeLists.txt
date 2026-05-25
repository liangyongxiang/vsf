# VSF Test Framework — CMake library
# Per vsf-test-generalization PRD: application no longer manually lists
# test source files; this directory is add_subdirectory'd once and
# discovers suites automatically.

vsf_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../source/component/test/vsf_test
)

# Core framework sources
vsf_add_sources(
    ${CMAKE_CURRENT_LIST_DIR}/../../source/component/test/vsf_test/vsf_test.c
    ${CMAKE_CURRENT_LIST_DIR}/../../source/component/test/vsf_test/vsf_test_shell.c
)

# Peripheral test suites — discovered automatically.
# Re-run CMake after adding a new .c file under any <periph>/ directory.
file(GLOB_RECURSE USART_SRCS ${CMAKE_CURRENT_LIST_DIR}/usart/*.c)
file(GLOB_RECURSE GPIO_SRCS  ${CMAKE_CURRENT_LIST_DIR}/gpio/*.c)
file(GLOB_RECURSE I2C_SRCS   ${CMAKE_CURRENT_LIST_DIR}/i2c/*.c)
file(GLOB_RECURSE RTC_SRCS   ${CMAKE_CURRENT_LIST_DIR}/rtc/*.c)
file(GLOB_RECURSE FLASH_SRCS ${CMAKE_CURRENT_LIST_DIR}/flash/*.c)
file(GLOB_RECURSE WDT_SRCS   ${CMAKE_CURRENT_LIST_DIR}/wdt/*.c)
file(GLOB_RECURSE ADC_SRCS   ${CMAKE_CURRENT_LIST_DIR}/adc/*.c)
file(GLOB_RECURSE PWM_SRCS   ${CMAKE_CURRENT_LIST_DIR}/pwm/*.c)
file(GLOB_RECURSE TIMER_SRCS ${CMAKE_CURRENT_LIST_DIR}/timer/*.c)
file(GLOB_RECURSE SPI_SRCS   ${CMAKE_CURRENT_LIST_DIR}/spi/*.c)
file(GLOB_RECURSE RNG_SRCS   ${CMAKE_CURRENT_LIST_DIR}/rng/*.c)
file(GLOB_RECURSE DMA_SRCS   ${CMAKE_CURRENT_LIST_DIR}/dma/*.c)

vsf_add_sources(
    ${USART_SRCS}
    ${GPIO_SRCS}
    ${I2C_SRCS}
    ${RTC_SRCS}
    ${FLASH_SRCS}
    ${WDT_SRCS}
    ${ADC_SRCS}
    ${PWM_SRCS}
    ${TIMER_SRCS}
    ${SPI_SRCS}
    ${RNG_SRCS}
    ${DMA_SRCS}
)
