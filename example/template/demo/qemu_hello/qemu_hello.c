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

#include "vsf.h"

static void __qemu_uart_init(void)
{
    CMSDK_UART0->CTRL = 0;
    CMSDK_UART0->BAUDDIV = 651;
    CMSDK_UART0->CTRL = CMSDK_UART_CTRL_TXEN_Msk | CMSDK_UART_CTRL_RXEN_Msk;
}

static void __qemu_uart_write(uint8_t ch)
{
    while (CMSDK_UART0->STATE & CMSDK_UART_STATE_TXBF_Msk) {
    }
    CMSDK_UART0->DATA = (uint32_t)ch;
}

static void __qemu_uart_puts(const char *str)
{
    while (*str != '\0') {
        if (*str == '\n') {
            __qemu_uart_write('\r');
        }
        __qemu_uart_write((uint8_t)*str++);
    }
}

#if defined(__QEMU_FAKE_SOC__) && (VSF_HAL_USE_USART == ENABLED)
static void __qemu_fake_soc_usart_puts(const char *str)
{
    vsf_usart_t *usart = (vsf_usart_t *)&vsf_hw_usart0;

    while (*str != '\0') {
        uint8_t ch = (uint8_t)*str++;

        if (ch == '\n') {
            uint8_t cr = '\r';
            while (vsf_usart_txfifo_write(usart, &cr, 1) != 1) {
            }
        }
        while (vsf_usart_txfifo_write(usart, &ch, 1) != 1) {
        }
    }
}

static void __qemu_fake_soc_usart_smoke_test(void)
{
    vsf_usart_t *usart = (vsf_usart_t *)&vsf_hw_usart0;
    vsf_usart_cfg_t cfg = {
        .mode = VSF_USART_NO_PARITY
              | VSF_USART_1_STOPBIT
              | VSF_USART_8_BIT_LENGTH
              | VSF_USART_NO_HWCONTROL
              | VSF_USART_TX_ENABLE
              | VSF_USART_RX_ENABLE
              | VSF_USART_SYNC_CLOCK_DISABLE
              | VSF_USART_HALF_DUPLEX_DISABLE
              | VSF_USART_TX_FIFO_THRESHOLD_EMPTY
              | VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY,
        .baudrate = 38400,
    };

    if (VSF_ERR_NONE != vsf_usart_init(usart, &cfg)) {
        __qemu_uart_puts("qemu fake soc usart init failed\n");
        return;
    }
    if (fsm_rt_cpl != vsf_usart_enable(usart)) {
        __qemu_uart_puts("qemu fake soc usart enable failed\n");
        return;
    }
    __qemu_fake_soc_usart_puts("qemu fake soc usart polling ok\n");
}
#endif

int main(void)
{
    vsf_driver_init();
    __qemu_uart_init();

#if defined(__QEMU_MPS2_BRIDGE__)
    __qemu_uart_puts("qemu mps2 bridge hello world\n");
#elif defined(__QEMU_FAKE_SOC__)
    __qemu_uart_puts("qemu fake soc hello world\n");
#else
    __qemu_uart_puts("qemu hello world\n");
#endif

#if defined(__QEMU_FAKE_SOC__) && (VSF_HAL_USE_USART == ENABLED)
    __qemu_fake_soc_usart_smoke_test();
#endif

    while (1) {
    }
}
