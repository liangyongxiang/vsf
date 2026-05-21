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

#if VSF_HAL_USE_DMA == ENABLED

#include "hal/vsf_hal.h"
#include "hardware/structs/dma.h"
#include "hardware/regs/dma.h"
#include "hardware/regs/resets.h"
#include "hardware/structs/resets.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_DMA_CFG_MULTI_CLASS
#   define VSF_HW_DMA_CFG_MULTI_CLASS              VSF_DMA_CFG_MULTI_CLASS
#endif

#define VSF_DMA_CFG_IMP_PREFIX                     vsf_hw
#define VSF_DMA_CFG_IMP_UPCASE_PREFIX              VSF_HW

#define RP2040_DMA_CHANNEL_COUNT                   12

/*============================ TYPES =========================================*/

typedef struct vsf_hw_dma_channel_t {
    vsf_dma_channel_cfg_t   cfg;
    vsf_dma_isr_t           isr;
    uint32_t                total_count;
} vsf_hw_dma_channel_t;

typedef struct VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) {
#if VSF_HW_DMA_CFG_MULTI_CLASS == ENABLED
    vsf_dma_t               vsf_dma;
#endif
    dma_hw_t                *reg;
    vsf_hw_dma_channel_t    channels[RP2040_DMA_CHANNEL_COUNT];
    uint16_t                channel_mask;
    vsf_dma_cfg_t           cfg;
} VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __rp2040_dma_reset(dma_hw_t *reg)
{
    uint32_t rst_bit = (1u << RESETS_RESET_DMA_LSB);
    resets_hw->reset |= rst_bit;
    resets_hw->reset &= ~rst_bit;
    while (!(resets_hw->reset_done & rst_bit));
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_init)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));

    __rp2040_dma_reset(dma_ptr->reg);

    dma_ptr->cfg = *cfg_ptr;
    dma_ptr->channel_mask = 0;
    for (uint8_t i = 0; i < RP2040_DMA_CHANNEL_COUNT; i++) {
        dma_ptr->channels[i].total_count = 0;
    }

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_fini)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    dma_hw_t *hw = dma_ptr->reg;
    for (uint8_t i = 0; i < RP2040_DMA_CHANNEL_COUNT; i++) {
        hw->ch[i].ctrl_trig = 0;
    }
    dma_ptr->channel_mask = 0;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_get_configuration)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));

    *cfg_ptr = dma_ptr->cfg;
    return VSF_ERR_NONE;
}

vsf_dma_capability_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_capability)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    return (vsf_dma_capability_t) {
        .irq_mask           = VSF_DMA_IRQ_MASK_CPL,
        .channel_count      = RP2040_DMA_CHANNEL_COUNT,
        .irq_count          = 2,
        .supported_modes    = VSF_DMA_MEMORY_TO_MEMORY
                            | VSF_DMA_MEMORY_TO_PERIPHERAL
                            | VSF_DMA_PERIPHERAL_TO_MEMORY
                            | VSF_DMA_SRC_ADDR_INCREMENT
                            | VSF_DMA_DST_ADDR_INCREMENT
                            | VSF_DMA_SRC_WIDTH_BYTE_1
                            | VSF_DMA_SRC_WIDTH_BYTES_2
                            | VSF_DMA_SRC_WIDTH_BYTES_4
                            | VSF_DMA_DST_WIDTH_BYTE_1
                            | VSF_DMA_DST_WIDTH_BYTES_2
                            | VSF_DMA_DST_WIDTH_BYTES_4,
        .max_transfer_count = 0xFFFFFFFF,
        .addr_alignment     = 1,
    };
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_acquire)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_channel_hint_t *channel_hint_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    uint8_t start = 0;
    uint8_t end = RP2040_DMA_CHANNEL_COUNT;

    if (channel_hint_ptr != NULL) {
        if (channel_hint_ptr->channel >= 0) {
            start = (uint8_t)channel_hint_ptr->channel;
            end = start + 1;
        }
    }

    for (uint8_t i = start; i < end; i++) {
        if (!(dma_ptr->channel_mask & (1u << i))) {
            dma_ptr->channel_mask |= (1u << i);
            if (channel_hint_ptr != NULL) {
                channel_hint_ptr->channel = (int8_t)i;
            }
            return VSF_ERR_NONE;
        }
    }

    return VSF_ERR_NOT_AVAILABLE;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_release)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < RP2040_DMA_CHANNEL_COUNT);

    dma_ptr->reg->ch[channel].ctrl_trig = 0;
    dma_ptr->channel_mask &= ~(1u << channel);
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_config)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));
    VSF_HAL_ASSERT(channel < RP2040_DMA_CHANNEL_COUNT);

    dma_ptr->channels[channel].cfg = *cfg_ptr;
    dma_ptr->channels[channel].isr = cfg_ptr->isr;

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_get_configuration)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));
    VSF_HAL_ASSERT(channel < RP2040_DMA_CHANNEL_COUNT);

    *cfg_ptr = dma_ptr->channels[channel].cfg;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_start)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel, vsf_dma_addr_t src_address,
    vsf_dma_addr_t dst_address, uint32_t count)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < RP2040_DMA_CHANNEL_COUNT);

    dma_hw_t *hw = dma_ptr->reg;
    dma_channel_hw_t *ch = &hw->ch[channel];

    ch->ctrl_trig = 0;

    ch->read_addr = (uint32_t)src_address;
    ch->write_addr = (uint32_t)dst_address;
    ch->transfer_count = count;

    uint32_t ctrl = DMA_CH0_CTRL_TRIG_EN_BITS;
    vsf_dma_channel_mode_t mode = dma_ptr->channels[channel].cfg.mode;

    /* Data size */
    switch (mode & VSF_DMA_SRC_WIDTH_MASK) {
    case VSF_DMA_SRC_WIDTH_BYTES_2:
        ctrl |= (1u << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB);
        break;
    case VSF_DMA_SRC_WIDTH_BYTES_4:
        ctrl |= (2u << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB);
        break;
    default:
        /* 8-bit default */
        break;
    }

    /* Address increment — INCREMENT has value 0 in VSF mode enum,
     * so we enable increment when neither DECREMENT nor NO_CHANGE is set. */
    switch (mode & (0x03u << 2)) {
    case VSF_DMA_SRC_ADDR_DECREMENT:
    case VSF_DMA_SRC_ADDR_NO_CHANGE:
        break;
    default:
        ctrl |= DMA_CH0_CTRL_TRIG_INCR_READ_BITS;
        break;
    }
    switch (mode & (0x03u << 4)) {
    case VSF_DMA_DST_ADDR_DECREMENT:
    case VSF_DMA_DST_ADDR_NO_CHANGE:
        break;
    default:
        ctrl |= DMA_CH0_CTRL_TRIG_INCR_WRITE_BITS;
        break;
    }

    /* TREQ: 0x3F = permanent request (mem2mem) */
    switch (mode & VSF_DMA_DIRECTION_MASK) {
    case VSF_DMA_MEMORY_TO_MEMORY:
        ctrl |= (0x3Fu << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);
        break;
    default:
        /* For peripheral modes, caller must set request_line in channel_hint */
        ctrl |= (0x3Fu << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);
        break;
    }

    ch->ctrl_trig = ctrl;

    dma_ptr->channels[channel].total_count = count;

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_cancel)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < RP2040_DMA_CHANNEL_COUNT);

    dma_ptr->reg->ch[channel].ctrl_trig = 0;
    return VSF_ERR_NONE;
}

uint32_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_get_transferred_count)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < RP2040_DMA_CHANNEL_COUNT);

    uint32_t remain = dma_ptr->reg->ch[channel].transfer_count;
    uint32_t total = dma_ptr->channels[channel].total_count;

    return total - remain;
}

vsf_dma_channel_status_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_status)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < RP2040_DMA_CHANNEL_COUNT);

    vsf_dma_channel_status_t status = {
        .is_busy = !!(dma_ptr->reg->ch[channel].ctrl_trig
                      & DMA_CH0_CTRL_TRIG_BUSY_BITS),
    };

    return status;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_ctrl)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    (void)ctrl;
    (void)param;
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

static void VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t irq_idx)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    dma_hw_t *hw = dma_ptr->reg;
    uint32_t ints = (irq_idx == 0) ? hw->ints0 : hw->ints1;

    for (uint8_t ch = 0; ch < RP2040_DMA_CHANNEL_COUNT; ch++) {
        if (ints & (1u << ch)) {
            /* Clear interrupt by writing to INTF */
            if (irq_idx == 0) {
                hw->intf0 = (1u << ch);
            } else {
                hw->intf1 = (1u << ch);
            }

            if (dma_ptr->channels[ch].isr.handler_fn != NULL) {
                dma_ptr->channels[ch].isr.handler_fn(
                    dma_ptr->channels[ch].isr.target_ptr,
                    (vsf_dma_t *)dma_ptr, ch, VSF_DMA_IRQ_MASK_CPL);
            }
        }
    }
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_GET_CONFIGURATION          ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_CHANNEL_GET_CONFIGURATION  ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_CTRL                       ENABLED

#define VSF_DMA_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)                                \
        VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX) = {                   \
        .reg = (dma_hw_t *)VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX,         \
                                         _DMA, __IDX, _REG),                    \
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX,               \
                      _DMA, __IDX, _IRQHandler)(void)                           \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(              \
            &VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX), 0);             \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/dma/dma_template.inc"

#endif      /* VSF_HAL_USE_DMA */
/* EOF */
