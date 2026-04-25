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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "hal/vsf_hal.h"

#define VSF_USART_CFG_IMP_PREFIX                vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX         VSF_HW
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST   ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CTRL      ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR ENABLED

/*\note Derived from source/hal/driver/template/__series_name_a__/common/usart/usart.c.
 *      The generic template provides the declaration/instance machinery while
 *      this file only fills in the fake-soc CMSDK APB UART specifics.
 */

/*============================ MACROS ========================================*/

#define VSF_QEMU_FAKE_SOC_USART_DEFAULT_BAUDRATE    38400UL
#define VSF_QEMU_FAKE_SOC_USART_PENDING_MASK        (   CMSDK_UART_CTRL_TXIRQ_Msk     \
                                                    |   CMSDK_UART_CTRL_RXIRQ_Msk     \
                                                    |   CMSDK_UART_CTRL_TXORIRQ_Msk   \
                                                    |   CMSDK_UART_CTRL_RXORIRQ_Msk)
#define VSF_QEMU_FAKE_SOC_USART_SUPPORTED_IRQ_MASK  (   VSF_USART_IRQ_MASK_TX         \
                                                    |   VSF_USART_IRQ_MASK_RX         \
                                                    |   VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR \
                                                    |   VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR)

/*============================ TYPES =========================================*/

typedef struct vsf_hw_usart_const_t {
    CMSDK_UART_TypeDef *reg;
    IRQn_Type rx_irqn;
    IRQn_Type tx_irqn;
    IRQn_Type ovf_irqn;
} vsf_hw_usart_const_t;

struct vsf_hw_usart_t {
#if VSF_HW_USART_CFG_MULTI_CLASS == ENABLED
    vsf_usart_t vsf_usart;
#endif
    vsf_usart_cfg_t cfg;
    vsf_usart_irq_mask_t irq_mask;
    bool is_enabled;
    const vsf_hw_usart_const_t *usart_const;
};

/*============================ PROTOTYPES ====================================*/

extern uint32_t SystemCoreClock;

/*============================ LOCAL FUNCTIONS ===============================*/

static uint32_t __vsf_hw_usart_get_clock_hz(void)
{
    return (SystemCoreClock != 0U) ? SystemCoreClock : 25000000UL;
}

static bool __vsf_hw_usart_mode_is_supported(vsf_usart_mode_t mode)
{
    if ((mode & VSF_USART_PARITY_MASK) != VSF_USART_NO_PARITY) {
        return false;
    }
    if ((mode & VSF_USART_STOPBIT_MASK) != VSF_USART_1_STOPBIT) {
        return false;
    }
    if ((mode & VSF_USART_BIT_LENGTH_MASK) != VSF_USART_8_BIT_LENGTH) {
        return false;
    }
    if ((mode & VSF_USART_HWCONTROL_MASK) != VSF_USART_NO_HWCONTROL) {
        return false;
    }
    if ((mode & VSF_USART_SYNC_CLOCK_MASK) != VSF_USART_SYNC_CLOCK_DISABLE) {
        return false;
    }
    if ((mode & VSF_USART_HALF_DUPLEX_MASK) != VSF_USART_HALF_DUPLEX_DISABLE) {
        return false;
    }
    if ((mode & VSF_USART_TX_FIFO_THRESHOLD_MASK) != VSF_USART_TX_FIFO_THRESHOLD_EMPTY) {
        return false;
    }
    if ((mode & VSF_USART_RX_FIFO_THRESHOLD_MASK) != VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY) {
        return false;
    }
    return true;
}

static uint32_t __vsf_hw_usart_calc_bauddiv(uint32_t baudrate)
{
    uint32_t clock_hz = __vsf_hw_usart_get_clock_hz();

    if (baudrate == 0U) {
        baudrate = VSF_QEMU_FAKE_SOC_USART_DEFAULT_BAUDRATE;
    }
    if (baudrate > clock_hz) {
        return 0U;
    }

    uint32_t bauddiv = (clock_hz + (baudrate / 2U)) / baudrate;
    if ((bauddiv == 0U) || (bauddiv > CMSDK_UART_BAUDDIV_Msk)) {
        return 0U;
    }
    return bauddiv;
}

static vsf_usart_irq_mask_t __vsf_hw_usart_pending_to_irq_mask(uint32_t pending)
{
    vsf_usart_irq_mask_t irq_mask = 0;

    if (pending & CMSDK_UART_CTRL_TXIRQ_Msk) {
        irq_mask |= VSF_USART_IRQ_MASK_TX;
    }
    if (pending & CMSDK_UART_CTRL_RXIRQ_Msk) {
        irq_mask |= VSF_USART_IRQ_MASK_RX;
    }
    if (pending & CMSDK_UART_CTRL_TXORIRQ_Msk) {
        irq_mask |= VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR;
    }
    if (pending & CMSDK_UART_CTRL_RXORIRQ_Msk) {
        irq_mask |= VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR;
    }
    return irq_mask;
}

static uint32_t __vsf_hw_usart_irq_mask_to_pending(vsf_usart_irq_mask_t irq_mask)
{
    uint32_t pending = 0;

    if (irq_mask & VSF_USART_IRQ_MASK_TX) {
        pending |= CMSDK_UART_CTRL_TXIRQ_Msk;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        pending |= CMSDK_UART_CTRL_RXIRQ_Msk;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR) {
        pending |= CMSDK_UART_CTRL_TXORIRQ_Msk;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR) {
        pending |= CMSDK_UART_CTRL_RXORIRQ_Msk;
    }
    return pending;
}

static void __vsf_hw_usart_disable_irq_line(IRQn_Type irqn)
{
    NVIC_DisableIRQ(irqn);
    NVIC_ClearPendingIRQ(irqn);
}

static void __vsf_hw_usart_apply_irq(vsf_hw_usart_t *usart_ptr)
{
    uint32_t ctrl = 0;
    bool has_handler = usart_ptr->cfg.isr.handler_fn != NULL;

    if (usart_ptr->is_enabled) {
        if ((usart_ptr->cfg.mode & VSF_USART_TX_DISABLE) == 0) {
            ctrl |= CMSDK_UART_CTRL_TXEN_Msk;
        }
        if ((usart_ptr->cfg.mode & VSF_USART_RX_DISABLE) == 0) {
            ctrl |= CMSDK_UART_CTRL_RXEN_Msk;
        }

        if (has_handler) {
            if (usart_ptr->irq_mask & VSF_USART_IRQ_MASK_TX) {
                ctrl |= CMSDK_UART_CTRL_TXIRQEN_Msk;
            }
            if (usart_ptr->irq_mask & VSF_USART_IRQ_MASK_RX) {
                ctrl |= CMSDK_UART_CTRL_RXIRQEN_Msk;
            }
            if (usart_ptr->irq_mask & VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR) {
                ctrl |= CMSDK_UART_CTRL_TXORIRQEN_Msk;
            }
            if (usart_ptr->irq_mask & VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR) {
                ctrl |= CMSDK_UART_CTRL_RXORIRQEN_Msk;
            }
        }
    }

    usart_ptr->usart_const->reg->CTRL = ctrl;

    if (usart_ptr->is_enabled && has_handler && (usart_ptr->irq_mask & VSF_USART_IRQ_MASK_RX)) {
        NVIC_EnableIRQ(usart_ptr->usart_const->rx_irqn);
    } else {
        __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->rx_irqn);
    }

    if (usart_ptr->is_enabled && has_handler && (usart_ptr->irq_mask & VSF_USART_IRQ_MASK_TX)) {
        NVIC_EnableIRQ(usart_ptr->usart_const->tx_irqn);
    } else {
        __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->tx_irqn);
    }

    if (    usart_ptr->is_enabled
        &&  has_handler
        &&  (usart_ptr->irq_mask & (VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR | VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR))) {
        NVIC_EnableIRQ(usart_ptr->usart_const->ovf_irqn);
    } else {
        __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->ovf_irqn);
    }
}

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(vsf_hw_usart_t *usart_ptr)
{
    vsf_usart_irq_mask_t irq_mask = vsf_hw_usart_irq_clear(usart_ptr, usart_ptr->irq_mask);
    vsf_usart_isr_t *isr_ptr = &usart_ptr->cfg.isr;

    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_usart_t *)usart_ptr, irq_mask);
    }
}

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_usart_init(vsf_hw_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    if (!__vsf_hw_usart_mode_is_supported(cfg_ptr->mode)) {
        return VSF_ERR_NOT_SUPPORT;
    }

    usart_ptr->cfg = *cfg_ptr;
    if (usart_ptr->cfg.baudrate == 0U) {
        usart_ptr->cfg.baudrate = VSF_QEMU_FAKE_SOC_USART_DEFAULT_BAUDRATE;
    }

    uint32_t bauddiv = __vsf_hw_usart_calc_bauddiv(usart_ptr->cfg.baudrate);
    if (bauddiv == 0U) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    usart_ptr->irq_mask = 0;
    usart_ptr->is_enabled = false;

    usart_ptr->usart_const->reg->CTRL = 0;
    usart_ptr->usart_const->reg->BAUDDIV = bauddiv;
    usart_ptr->usart_const->reg->INTCLEAR = VSF_QEMU_FAKE_SOC_USART_PENDING_MASK;

    __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->rx_irqn);
    __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->tx_irqn);
    __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->ovf_irqn);

    if (usart_ptr->cfg.isr.handler_fn != NULL) {
        NVIC_SetPriority(usart_ptr->usart_const->rx_irqn, usart_ptr->cfg.isr.prio);
        NVIC_SetPriority(usart_ptr->usart_const->tx_irqn, usart_ptr->cfg.isr.prio);
        NVIC_SetPriority(usart_ptr->usart_const->ovf_irqn, usart_ptr->cfg.isr.prio);
    }

    return VSF_ERR_NONE;
}

void vsf_hw_usart_fini(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    usart_ptr->irq_mask = 0;
    usart_ptr->is_enabled = false;
    usart_ptr->usart_const->reg->CTRL = 0;
    usart_ptr->usart_const->reg->INTCLEAR = VSF_QEMU_FAKE_SOC_USART_PENDING_MASK;

    __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->rx_irqn);
    __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->tx_irqn);
    __vsf_hw_usart_disable_irq_line(usart_ptr->usart_const->ovf_irqn);
}

vsf_err_t vsf_hw_usart_get_configuration(vsf_hw_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    *cfg_ptr = usart_ptr->cfg;
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_usart_enable(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    usart_ptr->is_enabled = true;
    __vsf_hw_usart_apply_irq(usart_ptr);
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_usart_disable(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    usart_ptr->is_enabled = false;
    __vsf_hw_usart_apply_irq(usart_ptr);
    return fsm_rt_cpl;
}

void vsf_hw_usart_irq_enable(vsf_hw_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT((irq_mask & ~VSF_QEMU_FAKE_SOC_USART_SUPPORTED_IRQ_MASK) == 0);

    usart_ptr->irq_mask |= irq_mask;
    __vsf_hw_usart_apply_irq(usart_ptr);
}

void vsf_hw_usart_irq_disable(vsf_hw_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    usart_ptr->irq_mask &= ~irq_mask;
    __vsf_hw_usart_apply_irq(usart_ptr);
}

vsf_usart_irq_mask_t vsf_hw_usart_irq_clear(vsf_hw_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    uint32_t pending = usart_ptr->usart_const->reg->INTSTATUS & VSF_QEMU_FAKE_SOC_USART_PENDING_MASK;
    vsf_usart_irq_mask_t pending_mask = __vsf_hw_usart_pending_to_irq_mask(pending) & irq_mask;
    uint32_t clear_mask = __vsf_hw_usart_irq_mask_to_pending(pending_mask);

    if (clear_mask != 0U) {
        usart_ptr->usart_const->reg->INTCLEAR = clear_mask;
    }
    return pending_mask;
}

vsf_usart_status_t vsf_hw_usart_status(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    uint32_t state = usart_ptr->usart_const->reg->STATE;
    return (vsf_usart_status_t) {
        .is_busy = ((state & CMSDK_UART_STATE_TXBF_Msk) != 0),
        .rx_error_detected = ((state & CMSDK_UART_STATE_RXOR_Msk) != 0),
        .tx_error_detected = ((state & CMSDK_UART_STATE_TXOR_Msk) != 0),
    };
}

vsf_usart_capability_t vsf_hw_usart_capability(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    uint32_t clock_hz = __vsf_hw_usart_get_clock_hz();
    uint32_t min_baudrate = clock_hz / CMSDK_UART_BAUDDIV_Msk;

    return (vsf_usart_capability_t) {
        .irq_mask = VSF_QEMU_FAKE_SOC_USART_SUPPORTED_IRQ_MASK,
        .max_baudrate = clock_hz,
        .min_baudrate = (min_baudrate != 0U) ? min_baudrate : 1U,
        .txfifo_depth = 1,
        .rxfifo_depth = 1,
        .max_data_bits = 8,
        .min_data_bits = 8,
        .support_rx_timeout = 0,
        .support_send_break = 0,
        .support_set_and_clear_break = 0,
        .support_sync_clock = 0,
    };
}

uint_fast32_t vsf_hw_usart_rxfifo_get_data_count(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    if (!usart_ptr->is_enabled) {
        return 0;
    }
    return (usart_ptr->usart_const->reg->STATE & CMSDK_UART_STATE_RXBF_Msk) != 0;
}

uint_fast32_t vsf_hw_usart_rxfifo_read(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT((buffer_ptr != NULL) || (count == 0));

    if (!usart_ptr->is_enabled) {
        return 0;
    }

    uint8_t *buffer = buffer_ptr;
    uint_fast32_t read_count = 0;
    while (read_count < count) {
        if ((usart_ptr->usart_const->reg->STATE & CMSDK_UART_STATE_RXBF_Msk) == 0) {
            break;
        }
        buffer[read_count++] = (uint8_t)usart_ptr->usart_const->reg->DATA;
    }
    return read_count;
}

uint_fast32_t vsf_hw_usart_txfifo_get_free_count(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);

    if (!usart_ptr->is_enabled) {
        return 0;
    }
    return (usart_ptr->usart_const->reg->STATE & CMSDK_UART_STATE_TXBF_Msk) == 0;
}

uint_fast32_t vsf_hw_usart_txfifo_write(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT((buffer_ptr != NULL) || (count == 0));

    if (!usart_ptr->is_enabled) {
        return 0;
    }

    uint8_t *buffer = buffer_ptr;
    uint_fast32_t write_count = 0;
    while (write_count < count) {
        if (usart_ptr->usart_const->reg->STATE & CMSDK_UART_STATE_TXBF_Msk) {
            break;
        }
        usart_ptr->usart_const->reg->DATA = buffer[write_count++];
    }
    return write_count;
}

vsf_err_t vsf_hw_usart_request_rx(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_UNUSED_PARAM(buffer_ptr);
    VSF_UNUSED_PARAM(count);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_hw_usart_cancel_rx(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    return VSF_ERR_NOT_SUPPORT;
}

int_fast32_t vsf_hw_usart_get_rx_count(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    return -1;
}

vsf_err_t vsf_hw_usart_request_tx(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_UNUSED_PARAM(buffer_ptr);
    VSF_UNUSED_PARAM(count);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_hw_usart_cancel_tx(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    return VSF_ERR_NOT_SUPPORT;
}

int_fast32_t vsf_hw_usart_get_tx_count(vsf_hw_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    return -1;
}

vsf_err_t vsf_hw_usart_ctrl(vsf_hw_usart_t *usart_ptr, vsf_usart_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_UNUSED_PARAM(ctrl);
    VSF_UNUSED_PARAM(param);
    return VSF_ERR_NOT_SUPPORT;
}

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                 \
    static const vsf_hw_usart_const_t vsf_hw_usart ## __IDX ## _const = {      \
        .reg = VSF_HW_USART ## __IDX ## _REG,                                   \
        .rx_irqn = VSF_HW_USART ## __IDX ## _RX_IRQN,                           \
        .tx_irqn = VSF_HW_USART ## __IDX ## _TX_IRQN,                           \
        .ovf_irqn = VSF_HW_USART ## __IDX ## _OVF_IRQN,                         \
    };                                                                          \
    vsf_hw_usart_t vsf_hw_usart ## __IDX = {                                    \
        __HAL_OP                                                                 \
        .cfg = {                                                                 \
            .mode = VSF_USART_NO_PARITY                                         \
                  | VSF_USART_1_STOPBIT                                          \
                  | VSF_USART_8_BIT_LENGTH                                       \
                  | VSF_USART_NO_HWCONTROL                                       \
                  | VSF_USART_TX_ENABLE                                          \
                  | VSF_USART_RX_ENABLE                                          \
                  | VSF_USART_SYNC_CLOCK_DISABLE                                 \
                  | VSF_USART_HALF_DUPLEX_DISABLE                                \
                  | VSF_USART_TX_FIFO_THRESHOLD_EMPTY                            \
                  | VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY,                       \
            .baudrate = VSF_QEMU_FAKE_SOC_USART_DEFAULT_BAUDRATE,               \
        },                                                                       \
        .usart_const = &vsf_hw_usart ## __IDX ## _const,                        \
    };                                                                          \
    void VSF_HW_USART ## __IDX ## _RX_IRQHandler(void)                          \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(          \
            &vsf_hw_usart ## __IDX                                              \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }                                                                           \
    void VSF_HW_USART ## __IDX ## _TX_IRQHandler(void)                          \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(          \
            &vsf_hw_usart ## __IDX                                              \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }                                                                           \
    void VSF_HW_USART ## __IDX ## _OVF_IRQHandler(void)                         \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(          \
            &vsf_hw_usart ## __IDX                                              \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    };
#include "hal/driver/common/usart/usart_template.inc"

#endif
/* EOF */
