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

    while (1) {
    }
}
