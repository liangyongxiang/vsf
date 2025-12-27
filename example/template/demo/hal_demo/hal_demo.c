/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED

#include "hal_demo.h"

#if APP_USE_LINUX_DEMO == ENABLED
#   include "shell/sys/linux/vsf_linux.h"
#endif

/*============================ IMPLEMENTATION ================================*/

#if APP_USE_LINUX_DEMO == ENABLED
int hal_main(void)
{

#   if APP_USE_HAL_ADC_DEMO == ENABLED && VSF_HAL_USE_ADC == ENABLED
    extern int adc_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/adc-test", adc_main);
#   endif
#   if APP_USE_HAL_FLASH_DEMO == ENABLED && VSF_HAL_USE_FLASH == ENABLED
    extern int flash_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/flash-test", flash_main);
#   endif
#   if APP_USE_HAL_GPIO_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int gpio_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/gpio-test", gpio_main);
#   endif
#   if APP_USE_HAL_I2C_DEMO == ENABLED && VSF_HAL_USE_I2C == ENABLED
    extern int i2c_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/i2c-test", i2c_main);
#   endif
#   if APP_USE_HAL_I2S_DEMO == ENABLED && VSF_HAL_USE_I2S == ENABLED
    extern int i2s_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/i2s-test", i2s_main);
#   endif
#   if APP_USE_HAL_IO_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int io_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/io-test", io_main);
#   endif
#   if APP_USE_HAL_MMC_DEMO == ENABLED && VSF_HAL_USE_MMC == ENABLED
    extern int mmc_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mmc-test", mmc_main);
#   endif
#   if APP_USE_HAL_PM_DEMO == ENABLED && VSF_HAL_USE_PM == ENABLED
    extern int pm_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/pm-test", pm_main);
#   endif
#   if APP_USE_HAL_PWM_DEMO == ENABLED && VSF_HAL_USE_PWM == ENABLED
    extern int pwm_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/pwm-test", pwm_main);
#   endif
#   if APP_USE_HAL_RNG_DEMO == ENABLED && VSF_HAL_USE_RNG == ENABLED
    extern int rng_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/rng-test", rng_main);
#   endif
#   if APP_USE_HAL_RTC_DEMO == ENABLED && VSF_HAL_USE_RTC == ENABLED
    extern int rtc_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/rtc-test", rtc_main);
#   endif
#   if APP_USE_HAL_SPI_DEMO == ENABLED && VSF_HAL_USE_SPI == ENABLED
    extern int spi_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/spi-test", spi_main);
#   endif
#   if APP_USE_HAL_TIMER_DEMO == ENABLED && VSF_HAL_USE_TIMER == ENABLED
    extern int timer_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/timer-test", timer_main);
#   endif
#   if APP_USE_HAL_USART_DEMO == ENABLED && VSF_HAL_USE_USART == ENABLED
    extern int usart_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/usart-test", usart_main);
#   endif
#   if APP_USE_HAL_WDT_DEMO == ENABLED && VSF_HAL_USE_WDT == ENABLED
    extern int wdt_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/wdt-test", wdt_main);
#   endif

    return 0;
}
#endif

#endif
