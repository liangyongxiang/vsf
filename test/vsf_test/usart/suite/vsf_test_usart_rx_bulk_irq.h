#ifndef __VSF_TEST_USART_RX_BULK_IRQ_H__
#define __VSF_TEST_USART_RX_BULK_IRQ_H__

#include "../vsf_test_usart.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_USART_RX_BULK_IRQ_BUF_SIZE
#   define VSF_TEST_USART_RX_BULK_IRQ_BUF_SIZE        4096
#endif

/*============================ TYPES =========================================*/

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
typedef struct {
    uint8_t rx_bulk_irq_buf[VSF_TEST_USART_RX_BULK_IRQ_BUF_SIZE];
    volatile bool done;
    uint8_t *dst;
    volatile uint32_t isr_count;
    uint_fast16_t received;
    uint_fast16_t target;
} vsf_test_usart_rx_bulk_irq_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_usart_rx_bulk_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_USART_RX_BULK_IRQ_H__ */
/* EOF */
