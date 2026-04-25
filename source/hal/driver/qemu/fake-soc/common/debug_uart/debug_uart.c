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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#   include "service/vsf_service.h"
#endif

#include "./debug_uart.h"

/*============================ MACROS ========================================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   ifndef VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE
#       define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE         32
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
static uint8_t __vsf_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_debug_stream_rx_buff,
    .size       = sizeof(__vsf_debug_stream_rx_buff),
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED

static void __uart_config(void)
{
    CMSDK_UART0->CTRL = 0;
    CMSDK_UART0->BAUDDIV = 651;
    CMSDK_UART0->CTRL = CMSDK_UART_CTRL_TXEN_Msk | CMSDK_UART_CTRL_RXEN_Msk;
}

static bool __uart_is_tx_full(void)
{
    return (CMSDK_UART0->STATE & CMSDK_UART_STATE_TXBF_Msk) != 0;
}

static void __uart_tx_write(uint8_t txchar)
{
    CMSDK_UART0->DATA = (uint32_t)txchar;
}

static bool __uart_is_rx_not_empty(void)
{
    return (CMSDK_UART0->STATE & CMSDK_UART_STATE_RXBF_Msk) != 0;
}

static uint8_t __uart_rx_read(void)
{
    return (uint8_t)CMSDK_UART0->DATA;
}

static void __VSF_DEBUG_STREAM_TX_INIT(void)
{
    __uart_config();
    vsf_stream_connect_tx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);
}

static void __VSF_DEBUG_STREAM_TX_WRITE_BLOCKED(uint8_t *buf, uint_fast32_t size)
{
    for (uint_fast32_t i = 0; i < size; i++) {
        while (__uart_is_tx_full());
        __uart_tx_write(*buf++);
    }
}

#include "hal/driver/common/debug_stream/debug_stream_tx_blocked.inc"

void VSF_DEBUG_STREAM_POLL(void)
{
    uint_fast32_t rx_free_size = VSF_STREAM_GET_FREE_SIZE(&VSF_DEBUG_STREAM_RX);
    uint8_t ch;

    while (rx_free_size && __uart_is_rx_not_empty()) {
        rx_free_size--;
        ch = __uart_rx_read();
        VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, &ch, 1);
    }
}
#endif

#endif
