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

#include "./usb.h"

#if VSF_USE_USB_DEVICE == ENABLED || VSF_USE_USB_HOST == ENABLED

// for usb_device_speed_t
#include "component/vsf_component.h"

#include "./usb_priv.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

// TODO: add vsf_hw_usbh_irq
#define __USB_OTG_IMPLEMENT(__N, __VALUE)                                       \
static const vsf_hw_usb_const_t __USB_OTG##__N##_const = {                      \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
vsf_hw_usb_t USB_OTG##__N##_IP = {                                              \
    .param = &__USB_OTG##__N##_const,                                           \
};                                                                              \
VSF_CAL_ROOT void USB_OTG##__N##_IRQHandler(void)                               \
{                                                                               \
    if (USB_OTG##__N##_IP.is_host) {                                            \
        vsf_hw_usbh_irq(&USB_OTG##__N##_IP);                                    \
    } else {                                                                    \
        vsf_hw_usbd_irq(&USB_OTG##__N##_IP);                                    \
    }                                                                           \
}

#define _USB_OTG_IMPLEMENT(__N, __VALUE)    __USB_OTG_IMPLEMENT(__N, __VALUE)
#define USB_OTG_IMPLEMENT(__N, __VALUE)     _USB_OTG_IMPLEMENT(__N, __VALUE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(vsf_hw_usbd_irq)
void vsf_hw_usbd_irq(vsf_hw_usb_t *dc) { VSF_HAL_ASSERT(false); }
VSF_CAL_WEAK(vsf_hw_usbh_irq)
void vsf_hw_usbh_irq(vsf_hw_usb_t *dc) { VSF_HAL_ASSERT(false); }

VSF_MREPEAT(USB_OTG_COUNT, USB_OTG_IMPLEMENT, NULL)

#endif
