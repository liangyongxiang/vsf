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
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "component/test/vsf_test/vsf_test.h"
#include "vsf_test_usart.h"

/*============================ IMPLEMENTATION ================================*/

#define REG_IF(gate, s, field, add_fn)            \
    VSF_TEST_REGISTER_SCENE(s, field, add_fn)

void vsf_test_usart_register_all(vsf_test_usart_scenes_t *s)
{
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    REG_IF("usart_baud",                s, baud,              vsf_test_usart_baud_add_cases);
#endif
#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    REG_IF("usart_mode",                s, mode,              vsf_test_usart_mode_add_cases);
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    REG_IF("usart_rx_data",             s, rx_data,           vsf_test_usart_rx_data_add_cases);
#endif
#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    REG_IF("usart_rx_baud",             s, rx_baud,           vsf_test_usart_rx_baud_add_cases);
#endif
#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    REG_IF("usart_rx_mode",             s, rx_mode,           vsf_test_usart_rx_mode_add_cases);
#endif
#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    REG_IF("usart_rx_irq",              s, rx_irq,            vsf_test_usart_rx_irq_add_cases);
#endif
#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    REG_IF("usart_rx_timeout",          s, rx_timeout,        vsf_test_usart_rx_timeout_add_cases);
#endif
#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
    REG_IF("usart_rx_parity_error",     s, rx_parity_error,   vsf_test_usart_rx_parity_error_add_cases);
#endif
#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
    REG_IF("usart_rx_frame_error",      s, rx_frame_error,    vsf_test_usart_rx_frame_error_add_cases);
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    REG_IF("usart_tx_fifo_irq",         s, tx_fifo_irq,       vsf_test_usart_tx_fifo_irq_add_cases);
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    REG_IF("usart_rx_fifo_irq",         s, rx_fifo_irq,       vsf_test_usart_rx_fifo_irq_add_cases);
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    REG_IF("usart_request_tx_irq",      s, request_tx_irq,    vsf_test_usart_request_tx_irq_add_cases);
#endif
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    REG_IF("usart_request_rx_irq",      s, request_rx_irq,    vsf_test_usart_request_rx_irq_add_cases);
#endif
#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
    REG_IF("usart_request_cancel",      s, request_cancel,    vsf_test_usart_request_cancel_add_cases);
#endif
}

/* EOF */
