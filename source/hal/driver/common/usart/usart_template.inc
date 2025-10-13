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

#if VSF_HAL_USE_USART == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#ifndef VSF_USART_CFG_REIMPLEMENT_API_REQUEST
#   define VSF_USART_CFG_REIMPLEMENT_API_REQUEST                    DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY                 DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_API_CTRL
#   define VSF_USART_CFG_REIMPLEMENT_API_CTRL                       DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION          DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR                  DISABLED
#endif

#ifdef VSF_USART_CFG_IMP_REMAP_PREFIX
#   undef VSF_USART_CFG_REIMPLEMENT_API_REQUEST
#   undef VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY
#   undef VSF_USART_CFG_REIMPLEMENT_API_CTRL
#   undef VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   undef VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_USART_CFG_REIMPLEMENT_API_REQUEST                    ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_API_CTRL                       ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION          ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR                  ENABLED
#endif

#if VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_USART_CFG_CAPABILITY_IRQ_MASK
#       define VSF_USART_CFG_CAPABILITY_IRQ_MASK                    VSF_USART_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE
#       define VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE                921600
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE
#       define VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE                4800
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_TXFIFO_DEPTH
#       define VSF_USART_CFG_CAPABILITY_TXFIFO_DEPTH                64
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_RXFIFO_DEPTH
#       define VSF_USART_CFG_CAPABILITY_RXFIFO_DEPTH                64
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS
#       define VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS               9
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS
#       define VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS               8
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT
#       define VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT          1
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_SUPPORT_SEND_BREAK
#       define VSF_USART_CFG_CAPABILITY_SUPPORT_SEND_BREAK          1
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_SUPPORT_SET_AND_CLEAR_BREAK
#       define VSF_USART_CFG_CAPABILITY_SUPPORT_SET_AND_CLEAR_BREAK 0
#   endif
#endif

#define vsf_real_usart_t                    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)
#define vsf_real_usart_request_rx           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_rx)
#define vsf_real_usart_cancel_rx            VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)
#define vsf_real_usart_get_rx_count         VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_rx_count)
#define vsf_real_usart_request_tx           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)
#define vsf_real_usart_cancel_tx            VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)
#define vsf_real_usart_get_tx_count         VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_tx_count)
#define vsf_real_usart_capability           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)
#define vsf_real_usart_ctrl                 VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_ctrl)
#define vsf_real_usart_get_configuration    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_configuration)
#define vsf_real_usart_irq_clear            VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_clear)
#define vsf_real_usart_tx_send_break        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_tx_send_break)

/*============================ IMPLEMENTATION ================================*/

#if VSF_USART_CFG_REIMPLEMENT_API_REQUEST == DISABLED

vsf_err_t vsf_real_usart_request_rx(vsf_real_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

vsf_err_t vsf_real_usart_cancel_rx(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

int_fast32_t vsf_real_usart_get_rx_count(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(0);
    return -1;
}

vsf_err_t vsf_real_usart_request_tx(vsf_real_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

vsf_err_t vsf_real_usart_cancel_tx(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

int_fast32_t vsf_real_usart_get_tx_count(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(0);
    return -1;
}

#endif

#if VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_usart_capability_t vsf_real_usart_capability(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);

    vsf_usart_capability_t usart_capability = {
        .irq_mask                       = VSF_USART_CFG_CAPABILITY_IRQ_MASK,
        .max_baudrate                   = VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE,
        .min_baudrate                   = VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE,
        .txfifo_depth                   = VSF_USART_CFG_CAPABILITY_TXFIFO_DEPTH,
        .rxfifo_depth                   = VSF_USART_CFG_CAPABILITY_RXFIFO_DEPTH,
        .max_data_bits                  = VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS,
        .min_data_bits                  = VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS,
        .support_rx_timeout             = VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT,
        .support_send_break             = VSF_USART_CFG_CAPABILITY_SUPPORT_SEND_BREAK,
        .support_set_and_clear_break    = VSF_USART_CFG_CAPABILITY_SUPPORT_SET_AND_CLEAR_BREAK,
    };

    return usart_capability;
}
#endif

#if VSF_USART_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_usart_ctrl(vsf_real_usart_t *usart_ptr, vsf_usart_ctrl_t ctrl, void * param)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_usart_get_configuration(vsf_real_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_usart_irq_mask_t vsf_real_usart_irq_clear(vsf_real_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_USART_CFG_CAPABILITY_IRQ_MASK
#undef VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE
#undef VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE
#undef VSF_USART_CFG_CAPABILITY_TXFIFO_DEPTH
#undef VSF_USART_CFG_CAPABILITY_RXFIFO_DEPTH
#undef VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS
#undef VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS
#undef VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT
#undef VSF_USART_CFG_REIMPLEMENT_API_REQUEST
#undef VSF_USART_CFG_REIMPLEMENT_API_CTRL
#undef VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR
#undef vsf_real_usart_t
#undef vsf_real_usart_init
#undef vsf_real_usart_fini
#undef vsf_real_usart_rxfifo_read
#undef vsf_real_usart_txfifo_write
#undef vsf_real_usart_irq_enable
#undef vsf_real_usart_irq_disable
#undef vsf_real_usart_request_rx
#undef vsf_real_usart_cancel_rx
#undef vsf_real_usart_get_rx_count
#undef vsf_real_usart_request_tx
#undef vsf_real_usart_cancel_tx
#undef vsf_real_usart_get_tx_count
#undef vsf_real_usart_capability
#undef vsf_real_usart_ctrl
#undef vsf_real_usart_get_configuration
#undef vsf_real_usart_irq_clear
#undef vsf_real_usart_tx_send_break

/*============================ IMPLEMENTATION ================================*/
/*============================ MACROS ========================================*/

#ifdef VSF_USART_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_usart_t                          VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)
#   define vsf_imp_usart_init                       VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)
#   define vsf_imp_usart_fini                       VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_fini)
#   define vsf_imp_usart_capability                 VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)
#   define vsf_imp_usart_enable                     VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_enable)
#   define vsf_imp_usart_disable                    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_disable)
#   define vsf_imp_usart_irq_enable                 VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)
#   define vsf_imp_usart_irq_disable                VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)
#   define vsf_imp_usart_irq_clear                  VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_clear)
#   define vsf_imp_usart_status                     VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_status)
#   define vsf_imp_usart_rxfifo_get_data_count      VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_get_data_count)
#   define vsf_imp_usart_rxfifo_read                VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)
#   define vsf_imp_usart_txfifo_get_free_count      VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_get_free_count)
#   define vsf_imp_usart_txfifo_write               VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)
#   define vsf_imp_usart_request_rx                 VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_rx)
#   define vsf_imp_usart_request_tx                 VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)
#   define vsf_imp_usart_cancel_rx                  VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)
#   define vsf_imp_usart_cancel_tx                  VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)
#   define vsf_imp_usart_get_rx_count               VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_rx_count)
#   define vsf_imp_usart_get_tx_count               VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_tx_count)
#   define vsf_imp_usart_ctrl                       VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_ctrl)
#   define vsf_imp_usart_get_configuration          VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_configuration)

#   define vsf_remap_usart_t                        VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_t)
#   define vsf_remap_usart_init                     VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_init)
#   define vsf_remap_usart_fini                     VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_fini)
#   define vsf_remap_usart_capability               VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_capability)
#   define vsf_remap_usart_enable                   VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_enable)
#   define vsf_remap_usart_disable                  VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_disable)
#   define vsf_remap_usart_irq_enable               VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_irq_enable)
#   define vsf_remap_usart_irq_disable              VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_irq_disable)
#   define vsf_remap_usart_irq_clear                VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_irq_clear)
#   define vsf_remap_usart_status                   VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_status)
#   define vsf_remap_usart_rxfifo_get_data_count    VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_rxfifo_get_data_count)
#   define vsf_remap_usart_rxfifo_read              VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_rxfifo_read)
#   define vsf_remap_usart_txfifo_get_free_count    VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_txfifo_get_free_count)
#   define vsf_remap_usart_txfifo_write             VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_txfifo_write)
#   define vsf_remap_usart_request_rx               VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_request_rx)
#   define vsf_remap_usart_request_tx               VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_request_tx)
#   define vsf_remap_usart_cancel_rx                VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_cancel_rx)
#   define vsf_remap_usart_cancel_tx                VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_cancel_tx)
#   define vsf_remap_usart_get_rx_count             VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_get_rx_count)
#   define vsf_remap_usart_get_tx_count             VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_get_tx_count)
#   define vsf_remap_usart_ctrl                     VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_ctrl)
#   define vsf_remap_usart_get_configuration        VSF_MCONNECT(VSF_USART_CFG_IMP_REMAP_PREFIX, _usart_get_configuration)
#   define VSF_USART_CFG_IMP_REMAP_FUNCTIONS                                                                        \
        vsf_err_t vsf_imp_usart_init(vsf_imp_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr)                          \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_init(usart_ptr, cfg_ptr);                                                        \
        }                                                                                                           \
        void vsf_imp_usart_fini(vsf_imp_usart_t *usart_ptr)                                                         \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            vsf_remap_usart_fini(usart_ptr);                                                                        \
        }                                                                                                           \
        fsm_rt_t vsf_imp_usart_enable(vsf_imp_usart_t *usart_ptr)                                                   \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_enable(usart_ptr);                                                               \
        }                                                                                                           \
        fsm_rt_t vsf_imp_usart_disable(vsf_imp_usart_t *usart_ptr)                                                  \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_disable(usart_ptr);                                                              \
        }                                                                                                           \
        void vsf_imp_usart_irq_enable(vsf_imp_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)                    \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            vsf_remap_usart_irq_enable(usart_ptr, irq_mask);                                                        \
        }                                                                                                           \
        void vsf_imp_usart_irq_disable(vsf_imp_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)                   \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            vsf_remap_usart_irq_disable(usart_ptr, irq_mask);                                                       \
        }                                                                                                           \
        vsf_usart_irq_mask_t vsf_imp_usart_irq_clear(vsf_imp_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)    \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_irq_clear(usart_ptr, irq_mask);                                                  \
        }                                                                                                           \
        vsf_usart_status_t vsf_imp_usart_status(vsf_imp_usart_t *usart_ptr)                                         \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_status(usart_ptr);                                                               \
        }                                                                                                           \
        vsf_usart_capability_t vsf_imp_usart_capability(vsf_imp_usart_t *usart_ptr)                                 \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_capability(usart_ptr);                                                           \
        }                                                                                                           \
        uint_fast16_t vsf_imp_usart_rxfifo_get_data_count(vsf_imp_usart_t *usart_ptr)                               \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_rxfifo_get_data_count(usart_ptr);                                                \
        }                                                                                                           \
        uint_fast16_t vsf_imp_usart_rxfifo_read(vsf_imp_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)  \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_rxfifo_read(usart_ptr, buffer_ptr, count);                                       \
        }                                                                                                           \
        uint_fast16_t vsf_imp_usart_txfifo_get_free_count(vsf_imp_usart_t *usart_ptr)                               \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_txfifo_get_free_count(usart_ptr);                                                \
        }                                                                                                           \
        uint_fast16_t vsf_imp_usart_txfifo_write(vsf_imp_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count) \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_txfifo_write(usart_ptr, buffer_ptr, count);                                      \
        }                                                                                                           \
        vsf_err_t vsf_imp_usart_request_rx(vsf_imp_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)       \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_request_rx(usart_ptr, buffer_ptr, count);                                        \
        }                                                                                                           \
        vsf_err_t vsf_imp_usart_request_tx(vsf_imp_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)       \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_request_tx(usart_ptr, buffer_ptr, count);                                        \
        }                                                                                                           \
        vsf_err_t vsf_imp_usart_cancel_rx(vsf_imp_usart_t *usart_ptr)                                               \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_cancel_rx(usart_ptr);                                                            \
        }                                                                                                           \
        vsf_err_t vsf_imp_usart_cancel_tx(vsf_imp_usart_t *usart_ptr)                                               \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_cancel_tx(usart_ptr);                                                            \
        }                                                                                                           \
        int_fast32_t vsf_imp_usart_get_rx_count(vsf_imp_usart_t *usart_ptr)                                         \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_get_rx_count(usart_ptr);                                                         \
        }                                                                                                           \
        int_fast32_t vsf_imp_usart_get_tx_count(vsf_imp_usart_t *usart_ptr)                                         \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_get_tx_count(usart_ptr);                                                         \
        }                                                                                                           \
        vsf_err_t vsf_imp_usart_ctrl(vsf_imp_usart_t *usart_ptr, vsf_usart_ctrl_t ctrl, void *param)                \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            return vsf_remap_usart_ctrl(usart_ptr, ctrl, param);                                                    \
        }                                                                                                           \
        vsf_err_t vsf_imp_usart_get_configuration(vsf_imp_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr)             \
        {                                                                                                           \
            VSF_HAL_ASSERT(usart_ptr != NULL);                                                                      \
            VSF_HAL_ASSERT(cfg_ptr != NULL);                                                                        \
            return vsf_remap_usart_get_configuration(usart_ptr, cfg_ptr);                                           \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _usart
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _USART

#if !defined(VSF_USART_CFG_IMP_PREFIX) && !defined(VSF_USART_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_USART_CFG_IMP_PREFIX in usart driver"
#endif

#if !defined(VSF_USART_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_USART_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_USART_CFG_IMP_UPCASE_PREFIX in usart driver"
#endif

#ifndef VSF_USART_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_USART_CFG_IMP_COUNT_MASK_PREFIX      VSF_USART_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_USART_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_USART_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_USART_CFG_IMP_PREFIX
#undef VSF_USART_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_USART_CFG_IMP_UPCASE_PREFIX
#undef VSF_USART_CFG_IMP_DEVICE_PREFIX
#undef VSF_USART_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_USART_CFG_IMP_REMAP_PREFIX
#undef VSF_USART_CFG_IMP_LV0
#undef VSF_USART_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_USART_CFG_IMP_HAS_OP
#undef VSF_USART_CFG_IMP_EXTERN_OP

#undef vsf_imp_usart_t
#undef vsf_imp_usart_init
#undef vsf_imp_usart_fini
#undef vsf_imp_usart_capability
#undef vsf_imp_usart_enable
#undef vsf_imp_usart_disable
#undef vsf_imp_usart_irq_enable
#undef vsf_imp_usart_irq_disable
#undef vsf_imp_usart_irq_clear
#undef vsf_imp_usart_status
#undef vsf_imp_usart_rxfifo_get_data_count
#undef vsf_imp_usart_rxfifo_read
#undef vsf_imp_usart_txfifo_get_free_count
#undef vsf_imp_usart_txfifo_write
#undef vsf_imp_usart_request_rx
#undef vsf_imp_usart_request_tx
#undef vsf_imp_usart_cancel_rx
#undef vsf_imp_usart_cancel_tx
#undef vsf_imp_usart_get_rx_count
#undef vsf_imp_usart_get_tx_count
#undef vsf_imp_usart_ctrl
#undef vsf_imp_usart_get_configuration
#undef vsf_imp_usart_tx_send_break

#undef vsf_remap_usart_t
#undef vsf_remap_usart_init
#undef vsf_remap_usart_fini
#undef vsf_remap_usart_capability
#undef vsf_remap_usart_enable
#undef vsf_remap_usart_disable
#undef vsf_remap_usart_irq_enable
#undef vsf_remap_usart_irq_disable
#undef vsf_remap_usart_irq_clear
#undef vsf_remap_usart_status
#undef vsf_remap_usart_rxfifo_get_data_count
#undef vsf_remap_usart_rxfifo_read
#undef vsf_remap_usart_txfifo_get_free_count
#undef vsf_remap_usart_txfifo_write
#undef vsf_remap_usart_request_rx
#undef vsf_remap_usart_request_tx
#undef vsf_remap_usart_cancel_rx
#undef vsf_remap_usart_cancel_tx
#undef vsf_remap_usart_get_rx_count
#undef vsf_remap_usart_get_tx_count
#undef vsf_remap_usart_ctrl
#undef vsf_remap_usart_get_configuration
#undef vsf_remap_usart_tx_send_break

/*============================ STATIC ASSERTIONS ==============================*/

/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_USART_PARITY_MODES
#   define VSF_USART_PARITY_MODES \
        VSF_USART_NO_PARITY, \
        VSF_USART_EVEN_PARITY, \
        VSF_USART_ODD_PARITY, \
        VSF_USART_FORCE_0_PARITY, \
        VSF_USART_FORCE_1_PARITY
#endif

#ifndef VSF_USART_STOP_BIT_MODES
#   define VSF_USART_STOP_BIT_MODES \
        VSF_USART_1_STOPBIT, \
        VSF_USART_1_5_STOPBIT, \
        VSF_USART_0_5_STOPBIT, \
        VSF_USART_2_STOPBIT
#endif

#ifndef VSF_USART_DATA_BIT_MODES
#   define VSF_USART_DATA_BIT_MODES \
        VSF_USART_5_BIT_LENGTH, \
        VSF_USART_6_BIT_LENGTH, \
        VSF_USART_7_BIT_LENGTH, \
        VSF_USART_8_BIT_LENGTH, \
        VSF_USART_9_BIT_LENGTH, \
        VSF_USART_10_BIT_LENGTH
#endif

#ifndef VSF_USART_HW_CONTROL_MODES
#   define VSF_USART_HW_CONTROL_MODES \
        VSF_USART_NO_HWCONTROL, \
        VSF_USART_RTS_HWCONTROL, \
        VSF_USART_CTS_HWCONTROL, \
        VSF_USART_RTS_CTS_HWCONTROL
#endif

#ifndef VSF_USART_TX_ENABLE_MODES
#   define VSF_USART_TX_ENABLE_MODES \
        VSF_USART_TX_ENABLE, \
        VSF_USART_TX_DISABLE
#endif

#ifndef VSF_USART_RX_ENABLE_MODES
#   define VSF_USART_RX_ENABLE_MODES \
        VSF_USART_RX_ENABLE, \
        VSF_USART_RX_DISABLE
#endif

#ifndef VSF_USART_SYNC_CLOCK_MODES
#   define VSF_USART_SYNC_CLOCK_MODES \
        VSF_USART_SYNC_CLOCK_ENABLE, \
        VSF_USART_SYNC_CLOCK_DISABLE
#endif

#ifndef VSF_USART_HALF_DUPLEX_MODES
#   define VSF_USART_HALF_DUPLEX_MODES \
        VSF_USART_HALF_DUPLEX_DISABLE, \
        VSF_USART_HALF_DUPLEX_ENABLE
#endif

#ifndef VSF_USART_TX_FIFO_THRESHOLD_MODES
#   define VSF_USART_TX_FIFO_THRESHOLD_MODES \
        VSF_USART_TX_FIFO_THRESHOLD_EMPTY, \
        VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY, \
        VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL
#endif

#ifndef VSF_USART_RX_FIFO_THRESHOLD_MODES
#   define VSF_USART_RX_FIFO_THRESHOLD_MODES \
        VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY, \
        VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL, \
        VSF_USART_RX_FIFO_THRESHOLD_FULL
#endif

#ifndef VSF_USART_SYNC_CLOCK_POLARITY_MODES
#   define VSF_USART_SYNC_CLOCK_POLARITY_MODES \
        VSF_USART_SYNC_CLOCK_POLARITY_LOW, \
        VSF_USART_SYNC_CLOCK_POLARITY_HIGH
#endif

#ifndef VSF_USART_SYNC_CLOCK_PHASE_MODES
#   define VSF_USART_SYNC_CLOCK_PHASE_MODES \
        VSF_USART_SYNC_CLOCK_PHASE_1_EDGE, \
        VSF_USART_SYNC_CLOCK_PHASE_2_EDGE
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check parity mode uniqueness - all parity modes should have different values */
VSF_HAL_CHECK_UNIQUE(VSF_USART_PARITY_MODES);

/* Check stop bit uniqueness - all stop bit modes should have different values */
VSF_HAL_CHECK_UNIQUE(VSF_USART_STOP_BIT_MODES);

/* Check data bit length uniqueness - all data bit length modes should have different values */
VSF_HAL_CHECK_UNIQUE(VSF_USART_DATA_BIT_MODES);

/* Check hardware flow control uniqueness - all hw control modes should have different values */
VSF_HAL_CHECK_UNIQUE(VSF_USART_HW_CONTROL_MODES);

/* Check TX enable/disable uniqueness */
VSF_HAL_CHECK_UNIQUE(VSF_USART_TX_ENABLE_MODES);

/* Check RX enable/disable uniqueness */
VSF_HAL_CHECK_UNIQUE(VSF_USART_RX_ENABLE_MODES);

/* Check sync clock enable/disable uniqueness */
VSF_HAL_CHECK_UNIQUE(VSF_USART_SYNC_CLOCK_MODES);

/* Check half duplex enable/disable uniqueness */
VSF_HAL_CHECK_UNIQUE(VSF_USART_HALF_DUPLEX_MODES);

/* Check TX FIFO threshold uniqueness - all TX FIFO threshold modes should have different values */
VSF_HAL_CHECK_UNIQUE(VSF_USART_TX_FIFO_THRESHOLD_MODES);

/* Check RX FIFO threshold uniqueness - all RX FIFO threshold modes should have different values */
VSF_HAL_CHECK_UNIQUE(VSF_USART_RX_FIFO_THRESHOLD_MODES);

/* Check sync clock polarity uniqueness */
VSF_HAL_CHECK_UNIQUE(VSF_USART_SYNC_CLOCK_POLARITY_MODES);

/* Check sync clock phase uniqueness */
VSF_HAL_CHECK_UNIQUE(VSF_USART_SYNC_CLOCK_PHASE_MODES);

/* ==================== CROSS-GROUP UNIQUENESS CHECKS ==================== */
/* Check uniqueness across different functional groups to prevent conflicts */

/* Cross-group uniqueness check - manually expand all default mode values */
/* This ensures no mode value conflicts between different functional groups */
/* Note: This only works for default configurations. Custom configurations need manual verification. */
VSF_HAL_CHECK_UNIQUE(
    VSF_USART_NO_PARITY, VSF_USART_EVEN_PARITY, VSF_USART_ODD_PARITY,
    VSF_USART_FORCE_0_PARITY, VSF_USART_FORCE_1_PARITY, VSF_USART_1_STOPBIT,
    VSF_USART_1_5_STOPBIT, VSF_USART_0_5_STOPBIT, VSF_USART_2_STOPBIT,
    VSF_USART_5_BIT_LENGTH, VSF_USART_6_BIT_LENGTH, VSF_USART_7_BIT_LENGTH,
    VSF_USART_8_BIT_LENGTH, VSF_USART_9_BIT_LENGTH, VSF_USART_10_BIT_LENGTH,
    VSF_USART_NO_HWCONTROL, VSF_USART_RTS_HWCONTROL, VSF_USART_CTS_HWCONTROL,
    VSF_USART_RTS_CTS_HWCONTROL, VSF_USART_TX_ENABLE, VSF_USART_TX_DISABLE,
    VSF_USART_RX_ENABLE, VSF_USART_RX_DISABLE, VSF_USART_SYNC_CLOCK_ENABLE,
    VSF_USART_SYNC_CLOCK_DISABLE, VSF_USART_HALF_DUPLEX_DISABLE,
    VSF_USART_HALF_DUPLEX_ENABLE, VSF_USART_TX_FIFO_THRESHOLD_EMPTY,
    VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY,
    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL, VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY,
    VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL, VSF_USART_RX_FIFO_THRESHOLD_FULL,
    VSF_USART_SYNC_CLOCK_POLARITY_LOW, VSF_USART_SYNC_CLOCK_POLARITY_HIGH,
    VSF_USART_SYNC_CLOCK_PHASE_1_EDGE, VSF_USART_SYNC_CLOCK_PHASE_2_EDGE);

#endif  /* VSF_HAL_USE_USART */
