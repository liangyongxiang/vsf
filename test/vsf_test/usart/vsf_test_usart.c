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

/*============================ GLOBAL VARIABLES ==============================*/

vsf_test_usart_suites_t vsf_test_usart_suites;

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_init(vsf_test_usart_suites_t *s,
                         const vsf_test_usart_suite_binding_t bindings[],
                         uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        vsf_test_usart_suite_base_t *suite = bindings[i].suite;
        vsf_usart_t                 *inst  = bindings[i].instance;
        if (inst == NULL) { continue; }

        suite->usart  = inst;
        suite->setup  = bindings[i].setup;
        suite->teardown = bindings[i].teardown;
    }
#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED
    vsf_test_usart_baud_add_cases(&s->baud);
#endif

#if VSF_TEST_USART_TX_MODE_ENABLE == ENABLED
    vsf_test_usart_mode_add_cases(&s->mode);
#endif

#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    vsf_test_usart_rx_data_add_cases(&s->rx_data);
#endif

#if VSF_TEST_USART_RX_BAUD_ENABLE == ENABLED
    vsf_test_usart_rx_baud_add_cases(&s->rx_baud);
#endif

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
    vsf_test_usart_rx_mode_add_cases(&s->rx_mode);
#endif

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_irq_add_cases(&s->rx_irq);
#endif

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
    vsf_test_usart_rx_timeout_add_cases(&s->rx_timeout);
#endif

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_parity_error_add_cases(&s->rx_parity_error);
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_frame_error_add_cases(&s->rx_frame_error);
#endif

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_break_error_add_cases(&s->rx_break_error);
#endif

#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
    vsf_test_usart_rx_overflow_error_add_cases(&s->rx_overflow_error);
#endif

#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
    vsf_test_usart_break_signal_add_cases(&s->break_signal);
#endif

#if VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE == ENABLED
    vsf_test_usart_hw_flow_control_add_cases(&s->hw_flow_control);
#endif

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_tx_fifo_irq_add_cases(&s->tx_fifo_irq);
#endif

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_irq_add_cases(&s->rx_fifo_irq);
#endif

#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_tx_irq_add_cases(&s->request_tx_irq);
#endif

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_rx_irq_add_cases(&s->request_rx_irq);
#endif

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
    vsf_test_usart_request_cancel_add_cases(&s->request_cancel);
#endif

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_bulk_irq_add_cases(&s->rx_bulk_irq);
#endif

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_threshold_add_cases(&s->rx_fifo_threshold);
#endif

}
