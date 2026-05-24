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

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite. No REG_IF wrapper
// needed.
void vsf_test_usart_init(vsf_test_usart_suites_t *s, const vsf_test_usart_cfg_t *cfg)
{
    s->baud.usart            = cfg->usart;
    s->mode.usart            = cfg->usart;
    s->rx_data.usart         = cfg->usart;
    s->rx_baud.usart         = cfg->usart;
    s->rx_mode.usart         = cfg->usart;
    s->rx_irq.usart          = cfg->usart;
    s->rx_timeout.usart      = cfg->usart;
    s->rx_parity_error.usart = cfg->usart;
    s->rx_frame_error.usart  = cfg->usart;
    s->rx_break_error.usart  = cfg->usart;
    s->rx_overflow_error.usart = cfg->usart;
    s->break_signal.usart      = cfg->usart;
    s->hw_flow_control.usart   = cfg->usart;
    s->tx_fifo_irq.usart     = cfg->usart;
    s->rx_fifo_irq.usart     = cfg->usart;
    s->request_tx_irq.usart  = cfg->usart;
    s->request_rx_irq.usart  = cfg->usart;
    s->request_cancel.usart  = cfg->usart;
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    s->rx_bulk_irq.usart     = cfg->usart;
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    s->rx_fifo_threshold.usart = cfg->usart;
#endif

    s->baud.setup            = cfg->setup;
    s->baud.teardown         = cfg->teardown;
    s->mode.setup            = cfg->setup;
    s->mode.teardown         = cfg->teardown;
    s->rx_data.setup         = cfg->setup;
    s->rx_data.teardown      = cfg->teardown;
    s->rx_baud.setup         = cfg->setup;
    s->rx_baud.teardown      = cfg->teardown;
    s->rx_mode.setup         = cfg->setup;
    s->rx_mode.teardown      = cfg->teardown;
    s->rx_irq.setup          = cfg->setup;
    s->rx_irq.teardown       = cfg->teardown;
    s->rx_timeout.setup      = cfg->setup;
    s->rx_timeout.teardown   = cfg->teardown;
    s->rx_parity_error.setup    = cfg->setup;
    s->rx_parity_error.teardown = cfg->teardown;
    s->rx_frame_error.setup     = cfg->setup;
    s->rx_frame_error.teardown  = cfg->teardown;
    s->rx_break_error.setup     = cfg->setup;
    s->rx_break_error.teardown  = cfg->teardown;
    s->rx_overflow_error.setup    = cfg->setup;
    s->rx_overflow_error.teardown = cfg->teardown;
    s->break_signal.setup       = cfg->setup;
    s->break_signal.teardown    = cfg->teardown;
    s->hw_flow_control.setup    = cfg->setup;
    s->hw_flow_control.teardown = cfg->teardown;
    s->tx_fifo_irq.setup        = cfg->setup;
    s->tx_fifo_irq.teardown     = cfg->teardown;
    s->rx_fifo_irq.setup        = cfg->setup;
    s->rx_fifo_irq.teardown     = cfg->teardown;
    s->request_tx_irq.setup     = cfg->setup;
    s->request_tx_irq.teardown  = cfg->teardown;
    s->request_rx_irq.setup     = cfg->setup;
    s->request_rx_irq.teardown  = cfg->teardown;
    s->request_cancel.setup     = cfg->setup;
    s->request_cancel.teardown  = cfg->teardown;
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    s->rx_bulk_irq.setup     = cfg->setup;
    s->rx_bulk_irq.teardown  = cfg->teardown;
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    s->rx_fifo_threshold.setup    = cfg->setup;
    s->rx_fifo_threshold.teardown = cfg->teardown;
#endif

    if (cfg->usart_fifo2req != NULL) {
        s->request_tx_irq.usart = cfg->usart_fifo2req;
        s->request_rx_irq.usart = cfg->usart_fifo2req;
        s->request_cancel.usart = cfg->usart_fifo2req;
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

/* EOF */
