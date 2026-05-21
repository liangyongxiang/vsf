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

#include "./gpio.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "hal/vsf_hal.h"

#include "hal/driver/vendor_driver.h"   /* for IO_IRQ_BANK0_IRQn + NVIC helpers */
#include "hardware/structs/io_bank0.h"
#include "hardware/structs/pads_bank0.h"
#include "hardware/structs/sio.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_GPIO_CFG_MULTI_CLASS
#   define VSF_HW_GPIO_CFG_MULTI_CLASS              VSF_GPIO_CFG_MULTI_CLASS
#endif

#define VSF_GPIO_CFG_IMP_PREFIX                     vsf_hw
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX              VSF_HW

#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION  ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER   ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET         ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR       ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SET                    ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR                  ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT              ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT             ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION       ENABLED

#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET      1
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR    1
#define VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_GPIO_OUTPUT_MODE 1

#define __RP2040_FUNCSEL_SIO                        5
#define __RP2040_FUNCSEL_NULL                       0x1F

/* The default vsf_gpio_mode_t enum keeps VSF_GPIO_AF commented out — drivers
 * that support AF must either redefine the enum or treat AF via
 * cfg.alternate_function. We use the latter convention: when alternate_function
 * is non-zero, the driver writes it directly into FUNCSEL regardless of the
 * mode base. Mode 5 (the slot the template reserves for VSF_GPIO_AF) is also
 * recognized for forward-compatibility with future redefinitions.
 */
#define __RP2040_VSF_GPIO_AF_VALUE                  (5 << 0)

#define __RP2040_PADS_OD                            (1u << 7)
#define __RP2040_PADS_IE                            (1u << 6)
#define __RP2040_PADS_PUE                           (1u << 3)
#define __RP2040_PADS_PDE                           (1u << 2)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_gpio_t {
#if VSF_HW_GPIO_CFG_MULTI_CLASS == ENABLED
    vsf_gpio_t              vsf_gpio;
#endif
    /* Track which pins were configured as OPEN_DRAIN so that gpio_write/set/
     * clear can emulate OD via SIO.gpio_oe (drive low / float for high).
     */
    vsf_gpio_pin_mask_t     open_drain_mask;

    /* EXTI: store the per-pin trigger bits and the user handler so the
     * IO_BANK0 ISR can dispatch. */
    vsf_gpio_exti_irq_cfg_t exti_cfg;
    /* Per-pin EXTI trigger bits, indexed by pin number. Each entry holds the
     * 4-bit mask (LEVEL_LOW=0, LEVEL_HIGH=1, EDGE_LOW=2, EDGE_HIGH=3) within
     * the IO_BANK0 INTR / PROC0_INTE registers. */
    uint8_t                 exti_trigger[VSF_HW_GPIO_PIN_COUNT];
} vsf_hw_gpio_t;

/*============================ IMPLEMENTATION ================================*/

static bool __rp2040_is_af_mode(vsf_gpio_mode_t mode, uint16_t alternate_function)
{
    return ((mode & VSF_GPIO_MODE_MASK) == __RP2040_VSF_GPIO_AF_VALUE)
        || (alternate_function != 0);
}

/*============================ IMPLEMENTATION ================================*/

/* Map VSF EXTI mode bits to the 4-bit RP2040 INTR/INTE field for a pin.
 * The 4 bits are: LEVEL_LOW=0, LEVEL_HIGH=1, EDGE_LOW=2, EDGE_HIGH=3. */
static uint8_t __rp2040_exti_trigger_from_mode(vsf_gpio_mode_t mode)
{
    vsf_gpio_mode_t exti_mode = mode & VSF_GPIO_EXTI_MODE_MASK;
    if (exti_mode == VSF_GPIO_EXTI_MODE_LOW_LEVEL)        return 0x1;  /* LEVEL_LOW */
    if (exti_mode == VSF_GPIO_EXTI_MODE_HIGH_LEVEL)       return 0x2;  /* LEVEL_HIGH */
    if (exti_mode == VSF_GPIO_EXTI_MODE_FALLING)          return 0x4;  /* EDGE_LOW */
    if (exti_mode == VSF_GPIO_EXTI_MODE_RISING)           return 0x8;  /* EDGE_HIGH */
    if (exti_mode == VSF_GPIO_EXTI_MODE_RISING_FALLING)   return 0xC;  /* both EDGE bits */
    return 0;
}

static uint32_t __rp2040_pads_value(vsf_gpio_mode_t mode)
{
    /* Build the PADS_BANK0 GPIOn register value from the mode bits.
     * Reset default is 0x56 (IE=1, DRIVE=01, SCHMITT=1, PDE=1). We replace
     * pull and IE/OD from the mode, while keeping DRIVE/SCHMITT/SLEWFAST.
     * Note: bit 2 (PDE) is intentionally NOT in the base — the user's pull
     * selection drives PUE/PDE explicitly.
     */
    uint32_t pads = 0x12;   /* DRIVE=01 (bit 4), SCHMITT=1 (bit 1) */

    vsf_gpio_mode_t base = mode & VSF_GPIO_MODE_MASK;
    if (base == VSF_GPIO_INPUT) {
        /* Input: enable input buffer. We do NOT set PADS.OD; the SIO.OE
         * bit alone determines whether the pin drives. This preserves the
         * ability to switch input→output atomically via SIO. */
        pads |= __RP2040_PADS_IE;
    } else if (base == VSF_GPIO_EXTI) {
        /* EXTI: keep input enabled, but DON'T disable output. The user may
         * combine EXTI with an output (e.g. self-trigger tests, open-drain
         * loopback). */
        pads |= __RP2040_PADS_IE;
    } else if (base == VSF_GPIO_OUTPUT_PUSH_PULL || base == VSF_GPIO_OUTPUT_OPEN_DRAIN) {
        /* RP2040 supports simultaneous output+input on the same pin, so
         * we keep IE=1. This is what backs `can_read_in_gpio_output_mode`
         * in the capability struct. */
        pads |= __RP2040_PADS_IE;
    } else if (base == VSF_GPIO_ANALOG) {
        /* Analog: input buffer off, output disabled */
        pads |= __RP2040_PADS_OD;
    } else {
        /* AF mode — leave IE on so the peripheral can read; OD off so it can drive */
        pads |= __RP2040_PADS_IE;
    }

    vsf_gpio_mode_t pull = mode & VSF_GPIO_PULL_UP_DOWN_MASK;
    if (pull == VSF_GPIO_PULL_UP) {
        pads |= __RP2040_PADS_PUE;
    } else if (pull == VSF_GPIO_PULL_DOWN) {
        pads |= __RP2040_PADS_PDE;
    }

    return pads;
}

static uint32_t __rp2040_funcsel(vsf_gpio_mode_t mode, uint16_t alternate_function)
{
    if (__rp2040_is_af_mode(mode, alternate_function)) {
        return alternate_function & 0x1F;
    }
    if ((mode & VSF_GPIO_MODE_MASK) == VSF_GPIO_ANALOG) {
        return __RP2040_FUNCSEL_NULL;
    }
    return __RP2040_FUNCSEL_SIO;
}

vsf_err_t vsf_hw_gpio_port_config_pins(vsf_hw_gpio_t *hw_gpio_ptr,
                                       vsf_gpio_pin_mask_t pin_mask,
                                       vsf_gpio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT((pin_mask & ~VSF_HW_GPIO_PIN_MASK) == 0);

    uint32_t funcsel = __rp2040_funcsel(cfg_ptr->mode, cfg_ptr->alternate_function);
    uint32_t pads    = __rp2040_pads_value(cfg_ptr->mode);
    vsf_gpio_mode_t base = cfg_ptr->mode & VSF_GPIO_MODE_MASK;
    bool is_output = (base == VSF_GPIO_OUTPUT_PUSH_PULL) || (base == VSF_GPIO_OUTPUT_OPEN_DRAIN);

    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        vsf_gpio_pin_mask_t bit = (vsf_gpio_pin_mask_t)1u << i;
        if (!(pin_mask & bit)) {
            continue;
        }
        pads_bank0_hw->io[i] = pads;
        io_bank0_hw->io[i].ctrl = funcsel;
    }

    /* Track open-drain pins for software emulation. */
    if (base == VSF_GPIO_OUTPUT_OPEN_DRAIN) {
        hw_gpio_ptr->open_drain_mask |= pin_mask;
    } else {
        hw_gpio_ptr->open_drain_mask &= ~pin_mask;
    }

    /* Track EXTI trigger bits per pin. exti_irq_enable() consumes these. */
    if (base == VSF_GPIO_EXTI) {
        uint8_t trig = __rp2040_exti_trigger_from_mode(cfg_ptr->mode);
        for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
            if (pin_mask & ((vsf_gpio_pin_mask_t)1u << i)) {
                hw_gpio_ptr->exti_trigger[i] = trig;
            }
        }
    } else {
        for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
            if (pin_mask & ((vsf_gpio_pin_mask_t)1u << i)) {
                hw_gpio_ptr->exti_trigger[i] = 0;
            }
        }
    }

    /* For SIO base modes, also set OE per direction. AF leaves OE controlled
     * by the peripheral via OEOVER/OEFROMPERI, so we don't touch SIO.OE here.
     */
    if (funcsel == __RP2040_FUNCSEL_SIO) {
        if (is_output && base == VSF_GPIO_OUTPUT_PUSH_PULL) {
            sio_hw->gpio_oe_set = pin_mask;
        } else if (base == VSF_GPIO_OUTPUT_OPEN_DRAIN) {
            /* OD emulation: OE starts cleared (line floats); writing 0 drives,
             * writing 1 releases. gpio_write handles direction toggling.
             */
            sio_hw->gpio_oe_clr = pin_mask;
        } else {
            /* INPUT / EXTI / ANALOG : OE cleared */
            sio_hw->gpio_oe_clr = pin_mask;
        }
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_gpio_get_pin_configuration(vsf_hw_gpio_t *hw_gpio_ptr,
                                            uint16_t pin_index,
                                            vsf_gpio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(pin_index < VSF_HW_GPIO_PIN_COUNT);

    uint32_t funcsel = io_bank0_hw->io[pin_index].ctrl & 0x1Fu;
    uint32_t pads    = pads_bank0_hw->io[pin_index];
    vsf_gpio_pin_mask_t bit = (vsf_gpio_pin_mask_t)1u << pin_index;

    /* Re-derive mode from registers + driver-side open-drain tracking. */
    vsf_gpio_mode_t mode;
    if (funcsel == __RP2040_FUNCSEL_NULL) {
        mode = VSF_GPIO_ANALOG;
    } else if (funcsel != __RP2040_FUNCSEL_SIO) {
        mode = __RP2040_VSF_GPIO_AF_VALUE;
    } else if (hw_gpio_ptr->open_drain_mask & bit) {
        mode = VSF_GPIO_OUTPUT_OPEN_DRAIN;
    } else if (sio_hw->gpio_oe & bit) {
        mode = VSF_GPIO_OUTPUT_PUSH_PULL;
    } else {
        mode = VSF_GPIO_INPUT;
    }

    if (pads & __RP2040_PADS_PUE) {
        mode |= VSF_GPIO_PULL_UP;
    } else if (pads & __RP2040_PADS_PDE) {
        mode |= VSF_GPIO_PULL_DOWN;
    } else {
        mode |= VSF_GPIO_NO_PULL_UP_DOWN;
    }

    cfg_ptr->mode               = mode;
    cfg_ptr->alternate_function = (funcsel == __RP2040_FUNCSEL_SIO || funcsel == __RP2040_FUNCSEL_NULL)
                                  ? 0 : funcsel;
    return VSF_ERR_NONE;
}

void vsf_hw_gpio_set_direction(vsf_hw_gpio_t *hw_gpio_ptr,
                               vsf_gpio_pin_mask_t pin_mask,
                               vsf_gpio_pin_mask_t direction_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    vsf_gpio_pin_mask_t out_mask = pin_mask & direction_mask;
    vsf_gpio_pin_mask_t in_mask  = pin_mask & ~direction_mask;
    if (out_mask) {
        sio_hw->gpio_oe_set = out_mask;
    }
    if (in_mask) {
        sio_hw->gpio_oe_clr = in_mask;
    }
}

vsf_gpio_pin_mask_t vsf_hw_gpio_get_direction(vsf_hw_gpio_t *hw_gpio_ptr,
                                              vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    return sio_hw->gpio_oe & pin_mask;
}

void vsf_hw_gpio_set_input(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_oe_clr = pin_mask;
}

void vsf_hw_gpio_set_output(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_oe_set = pin_mask;
}

void vsf_hw_gpio_switch_direction(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_oe_togl = pin_mask;
}

vsf_gpio_pin_mask_t vsf_hw_gpio_read(vsf_hw_gpio_t *hw_gpio_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    return sio_hw->gpio_in;
}

vsf_gpio_pin_mask_t vsf_hw_gpio_read_output_register(vsf_hw_gpio_t *hw_gpio_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    return sio_hw->gpio_out;
}

void vsf_hw_gpio_write(vsf_hw_gpio_t *hw_gpio_ptr,
                       vsf_gpio_pin_mask_t pin_mask,
                       vsf_gpio_pin_mask_t value)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    /* Push-pull pins in pin_mask: drive via gpio_out atomic set/clr. */
    vsf_gpio_pin_mask_t pp_mask = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    if (pp_mask) {
        vsf_gpio_pin_mask_t set_bits = pp_mask & value;
        vsf_gpio_pin_mask_t clr_bits = pp_mask & ~value;
        if (set_bits) sio_hw->gpio_set = set_bits;
        if (clr_bits) sio_hw->gpio_clr = clr_bits;
    }
    /* Open-drain pins: pre-program gpio_out=0, toggle OE to drive (low) or
     * float (high). gpio_out for OD pins stays at 0 across calls.
     */
    vsf_gpio_pin_mask_t od_mask = pin_mask & hw_gpio_ptr->open_drain_mask;
    if (od_mask) {
        sio_hw->gpio_clr = od_mask;
        vsf_gpio_pin_mask_t drive_low = od_mask & ~value;
        vsf_gpio_pin_mask_t release   = od_mask & value;
        if (drive_low) sio_hw->gpio_oe_set = drive_low;
        if (release)   sio_hw->gpio_oe_clr = release;
    }
}

void vsf_hw_gpio_set(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_hw_gpio_write(hw_gpio_ptr, pin_mask, pin_mask);
}

void vsf_hw_gpio_clear(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_hw_gpio_write(hw_gpio_ptr, pin_mask, 0);
}

void vsf_hw_gpio_toggle(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    vsf_gpio_pin_mask_t pp_mask = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    if (pp_mask) {
        sio_hw->gpio_togl = pp_mask;
    }
    /* OD pins toggle direction (drive ↔ float) since gpio_out stays at 0. */
    vsf_gpio_pin_mask_t od_mask = pin_mask & hw_gpio_ptr->open_drain_mask;
    if (od_mask) {
        sio_hw->gpio_oe_togl = od_mask;
    }
}

void vsf_hw_gpio_output_and_set(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    /* SIO atomic: program gpio_out before enabling OE, so the pin transitions
     * from input (float) directly to output-high with no intermediate state.
     */
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_set    = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    sio_hw->gpio_oe_set = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    /* For OD pins, "set" means release (float), i.e. clear OE. */
    sio_hw->gpio_oe_clr = pin_mask & hw_gpio_ptr->open_drain_mask;
}

void vsf_hw_gpio_output_and_clear(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_clr    = pin_mask;
    sio_hw->gpio_oe_set = pin_mask;
}

vsf_gpio_capability_t vsf_hw_gpio_capability(vsf_hw_gpio_t *hw_gpio_ptr)
{
    return (vsf_gpio_capability_t){
        .is_async                       = 0,
        .support_output_and_set         = 1,
        .support_output_and_clear       = 1,
        .support_interrupt              = 1,
        .can_read_in_gpio_output_mode   = 1,
        .can_read_in_alternate_mode     = 1,
        .pin_count                      = VSF_HW_GPIO_PIN_COUNT,
        .pin_mask                       = VSF_HW_GPIO_PIN_MASK,
    };
}

vsf_err_t vsf_hw_gpio_exti_irq_config(vsf_hw_gpio_t *hw_gpio_ptr,
                                      vsf_gpio_exti_irq_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    hw_gpio_ptr->exti_cfg = *cfg_ptr;
    NVIC_SetPriority(IO_IRQ_BANK0_IRQn, (uint32_t)cfg_ptr->prio);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_gpio_exti_irq_get_configuration(vsf_hw_gpio_t *hw_gpio_ptr,
                                                 vsf_gpio_exti_irq_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    *cfg_ptr = hw_gpio_ptr->exti_cfg;
    return VSF_ERR_NONE;
}

/* Map VSF EXTI mode bits to the 4-bit RP2040 INTR/INTE field for a pin.
 * Used by exti_irq_enable() to compute proc0_irq_ctrl.inte bits.
 * Definition is at the top of this file. */

vsf_err_t vsf_hw_gpio_exti_irq_enable(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        if (!(pin_mask & ((vsf_gpio_pin_mask_t)1u << i))) { continue; }
        uint8_t trig = hw_gpio_ptr->exti_trigger[i];
        if (trig == 0) { continue; }
        uint32_t reg_index = i >> 3;
        uint32_t bit_shift = (i & 7) * 4;
        /* Clear any stale edge status before enabling. */
        io_bank0_hw->intr[reg_index] = (uint32_t)(trig & 0xC) << bit_shift;
        io_bank0_hw->proc0_irq_ctrl.inte[reg_index] |= (uint32_t)trig << bit_shift;
    }
    NVIC_EnableIRQ(IO_IRQ_BANK0_IRQn);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_gpio_exti_irq_disable(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        if (!(pin_mask & ((vsf_gpio_pin_mask_t)1u << i))) { continue; }
        uint32_t reg_index = i >> 3;
        uint32_t bit_shift = (i & 7) * 4;
        io_bank0_hw->proc0_irq_ctrl.inte[reg_index] &= ~(0xFu << bit_shift);
    }
    /* Leave NVIC enabled — other pins may still need IRQs. */
    return VSF_ERR_NONE;
}

vsf_gpio_pin_mask_t vsf_hw_gpio_exti_irq_clear(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    vsf_gpio_pin_mask_t pending = 0;
    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        if (!(pin_mask & ((vsf_gpio_pin_mask_t)1u << i))) { continue; }
        uint32_t reg_index = i >> 3;
        uint32_t bit_shift = (i & 7) * 4;
        uint32_t pin_status = (io_bank0_hw->intr[reg_index] >> bit_shift) & 0xFu;
        if (pin_status) {
            pending |= (vsf_gpio_pin_mask_t)1u << i;
        }
        /* Only edge bits are write-1-to-clear; level bits auto-track. */
        io_bank0_hw->intr[reg_index] = (uint32_t)(pin_status & 0xC) << bit_shift;
    }
    return pending;
}

/* IO_BANK0 IRQ handler. Aggregates all 30 GPIO interrupts. */
void IO_BANK0_IRQHandler(void)
{
    uintptr_t ctx = vsf_hal_irq_enter();
    vsf_gpio_pin_mask_t fired = 0;
    /* Read all 4 INTS words; one bit per (pin, type). Reduce to pin mask
     * and clear edge bits via INTR (level bits auto-track). */
    for (uint32_t reg_index = 0; reg_index < 4; reg_index++) {
        uint32_t status = io_bank0_hw->proc0_irq_ctrl.ints[reg_index];
        if (!status) { continue; }
        /* Clear edge bits we observed (level bits self-clear when condition ends). */
        io_bank0_hw->intr[reg_index] = status & 0xCCCCCCCCu;
        for (uint32_t slot = 0; slot < 8; slot++) {
            if (status & (0xFu << (slot * 4))) {
                uint32_t pin = reg_index * 8 + slot;
                if (pin < VSF_HW_GPIO_PIN_COUNT) {
                    fired |= (vsf_gpio_pin_mask_t)1u << pin;
                }
            }
        }
    }
    if (fired && vsf_hw_gpio0.exti_cfg.handler_fn != NULL) {
        vsf_hw_gpio0.exti_cfg.handler_fn(vsf_hw_gpio0.exti_cfg.target_ptr,
                                         (vsf_gpio_t *)&vsf_hw_gpio0, fired);
    }
    vsf_hal_irq_leave(ctx);
}

vsf_err_t vsf_hw_gpio_ctrl(vsf_hw_gpio_t *hw_gpio_ptr, vsf_gpio_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_REIMPLEMENT_API_CTRL                       ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR             ENABLED

#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                       \
    vsf_hw_gpio_t vsf_hw_gpio ## __IDX = {                                          \
        __HAL_OP                                                                    \
    };

#include "hal/driver/common/gpio/gpio_template.inc"

#endif      /* VSF_HAL_USE_GPIO */
