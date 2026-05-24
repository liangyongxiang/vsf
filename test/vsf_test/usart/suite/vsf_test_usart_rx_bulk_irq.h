#ifndef __VSF_TEST_USART_RX_BULK_IRQ_H__
#define __VSF_TEST_USART_RX_BULK_IRQ_H__

#include "../vsf_test_usart.h"

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

void vsf_test_usart_rx_bulk_irq_run(const vsf_test_usart_rx_bulk_irq_case_t *c);

#ifdef __cplusplus
}
#endif

#endif

#endif /* __VSF_TEST_USART_RX_BULK_IRQ_H__ */
/* EOF */
