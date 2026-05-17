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

#include "./uart.h"

#if VSF_HAL_USE_USART == ENABLED

#define __VSF_HAL_PL011_UART_CLASS_INHERIT__

#include "hal/vsf_hal.h"

#include "RP2040.h"
#include "hardware/structs/resets.h"
#include "hal/driver/IPCore/ARM/PL011/vsf_pl011_uart_reg.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS         VSF_USART_CFG_MULTI_CLASS
#endif

#define VSF_USART_CFG_IMP_PREFIX                vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX         VSF_HW

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) {
    implement(vsf_pl011_usart_t)
    IRQn_Type               irqn;
} VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    uint32_t rst_bit = (usart_ptr->irqn == UART1_IRQ_IRQn)
                       ? (1u << RESET_UART1) : (1u << RESET_UART0);
    resets_hw->reset = resets_hw->reset & ~rst_bit;
    while (!(resets_hw->reset_done & rst_bit));

    vsf_err_t err = vsf_pl011_usart_init(
        &usart_ptr->use_as__vsf_pl011_usart_t, cfg_ptr, clock_get_hz(clk_peri));

    vsf_usart_isr_t *isr_ptr = &cfg_ptr->isr;
    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(usart_ptr->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(usart_ptr->irqn);
    } else {
        NVIC_DisableIRQ(usart_ptr->irqn);
    }

    return err;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_fini)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_fini(&usart_ptr->use_as__vsf_pl011_usart_t);
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_enable(&usart_ptr->use_as__vsf_pl011_usart_t);
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_disable(&usart_ptr->use_as__vsf_pl011_usart_t);
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_irq_enable(&usart_ptr->use_as__vsf_pl011_usart_t, irq_mask);
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_irq_disable(&usart_ptr->use_as__vsf_pl011_usart_t, irq_mask);
}

vsf_usart_irq_mask_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_clear)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_reg_t *reg = (vsf_pl011_usart_reg_t *)usart_ptr->reg;
    reg->UARTICR.VALUE = irq_mask & PL011_USART_IRQ_MASK;
    return irq_mask & PL011_USART_IRQ_MASK;
}

vsf_usart_status_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_status)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_status(&usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_get_data_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_rxfifo_get_data_count(
        &usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);
    return vsf_pl011_usart_rxfifo_read(
        &usart_ptr->use_as__vsf_pl011_usart_t, buffer_ptr, (uint_fast16_t)count);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_get_free_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return vsf_pl011_usart_txfifo_get_free_count(
        &usart_ptr->use_as__vsf_pl011_usart_t);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);
    return vsf_pl011_usart_txfifo_write(
        &usart_ptr->use_as__vsf_pl011_usart_t, buffer_ptr, (uint_fast16_t)count);
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_rx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_rxdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, true);
    // TODO: setup DMA
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_txdma_config(&usart_ptr->use_as__vsf_pl011_usart_t, true);
    // TODO: setup DMA
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NONE;
}

int_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_rx_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return 0;
}

int_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_tx_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return 0;
}

vsf_usart_capability_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_usart_capability_t cap = vsf_pl011_usart_capability(
        &usart_ptr->use_as__vsf_pl011_usart_t, clock_get_hz(clk_peri));
    cap.irq_mask |= VSF_USART_IRQ_MASK_TX_CPL | VSF_USART_IRQ_MASK_RX_CPL;
    return cap;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_ctrl)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_configuration)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    cfg_ptr->mode       = VSF_USART_8_BIT_LENGTH
                        | VSF_USART_1_STOPBIT
                        | VSF_USART_NO_PARITY;
    cfg_ptr->baudrate   = 115200;
    cfg_ptr->rx_timeout = 0;
    cfg_ptr->isr        = usart_ptr->isr;
    return VSF_ERR_NONE;
}

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_pl011_usart_irqhandler(&usart_ptr->use_as__vsf_pl011_usart_t);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_USART_CFG_MODE_CHECK_UNIQUE                 VSF_HAL_CHECK_MODE_LOOSE
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST           ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CTRL              ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR         ENABLED

#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)                            \
        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX) = {               \
        .reg  = (void *)VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,           \
                                     _USART, __IDX, _REG),                      \
        .irqn = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,                   \
                             _USART, __IDX, _IRQN),                             \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,                          \
                      _USART, __IDX, _IRQHandler)(void)                         \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(          \
            &VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX)              \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/usart/usart_template.inc"

#endif      // VSF_HAL_USE_USART
/* EOF */
