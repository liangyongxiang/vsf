/******************************************************************************
 * @file     startup_ARMCM7.c(modified for STM32H7)
 * @brief    CMSIS-Core(M) Device Startup File for a Cortex-M7 Device
 * @version  V2.0.0
 * @date     04. June 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../__device.h"

#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/driver.h"

// for VSF_MFOREACH
#include "utilities/vsf_utilities.h"

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/

void __NO_RETURN Reset_Handler  (void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
VSF_CAL_WEAK(NMI_Handler)
void NMI_Handler            (void){}

VSF_CAL_WEAK(HardFault_Handler)
void HardFault_Handler      (void){while(1);}

VSF_CAL_WEAK(MemManage_Handler)
void MemManage_Handler      (void){while(1);}

VSF_CAL_WEAK(BusFault_Handler)
void BusFault_Handler       (void){while(1);}

VSF_CAL_WEAK(UsageFault_Handler)
void UsageFault_Handler     (void){while(1);}

VSF_CAL_WEAK(SVC_Handler)
void SVC_Handler            (void){}

VSF_CAL_WEAK(DebugMon_Handler)
void DebugMon_Handler       (void){}

VSF_CAL_WEAK(PendSV_Handler)
void PendSV_Handler         (void){}

VSF_CAL_WEAK(SysTick_Handler)
void SysTick_Handler        (void){}

VSF_CAL_WEAK(WWDG_IRQHandler)
void WWDG_IRQHandler        (void){}

VSF_CAL_WEAK(PVD_AVD_IRQHandler)
void PVD_AVD_IRQHandler     (void){}

VSF_CAL_WEAK(TAMP_STAMP_IRQHandler)
void TAMP_STAMP_IRQHandler  (void){}

VSF_CAL_WEAK(RTC_WKUP_IRQHandler)
void RTC_WKUP_IRQHandler    (void){}

VSF_CAL_WEAK(FLASH_IRQHandler)
void FLASH_IRQHandler       (void){}

VSF_CAL_WEAK(RCC_IRQHandler)
void RCC_IRQHandler         (void){}

VSF_CAL_WEAK(EXTI0_IRQHandler)
void EXTI0_IRQHandler       (void){}

VSF_CAL_WEAK(EXTI1_IRQHandler)
void EXTI1_IRQHandler       (void){}

VSF_CAL_WEAK(EXTI2_IRQHandler)
void EXTI2_IRQHandler       (void){}

VSF_CAL_WEAK(EXTI3_IRQHandler)
void EXTI3_IRQHandler       (void){}

VSF_CAL_WEAK(EXTI4_IRQHandler)
void EXTI4_IRQHandler       (void){}

VSF_CAL_WEAK(DMA1_Stream0_IRQHandler)
void DMA1_Stream0_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Stream1_IRQHandler)
void DMA1_Stream1_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Stream2_IRQHandler)
void DMA1_Stream2_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Stream3_IRQHandler)
void DMA1_Stream3_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Stream4_IRQHandler)
void DMA1_Stream4_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Stream5_IRQHandler)
void DMA1_Stream5_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Stream6_IRQHandler)
void DMA1_Stream6_IRQHandler(void){}

VSF_CAL_WEAK(ADC_IRQHandler)
void ADC_IRQHandler         (void){}

VSF_CAL_WEAK(FDCAN1_IT0_IRQHandler)
void FDCAN1_IT0_IRQHandler  (void){}

VSF_CAL_WEAK(FDCAN2_IT0_IRQHandler)
void FDCAN2_IT0_IRQHandler  (void){}

VSF_CAL_WEAK(FDCAN1_IT1_IRQHandler)
void FDCAN1_IT1_IRQHandler  (void){}

VSF_CAL_WEAK(FDCAN2_IT1_IRQHandler)
void FDCAN2_IT1_IRQHandler  (void){}

VSF_CAL_WEAK(EXTI9_5_IRQHandler)
void EXTI9_5_IRQHandler     (void){}

VSF_CAL_WEAK(TIM1_BRK_IRQHandler)
void TIM1_BRK_IRQHandler    (void){}

VSF_CAL_WEAK(TIM1_UP_IRQHandler)
void TIM1_UP_IRQHandler     (void){}

VSF_CAL_WEAK(TIM1_TRG_COM_IRQHandler)
void TIM1_TRG_COM_IRQHandler(void){}

VSF_CAL_WEAK(TIM1_CC_IRQHandler)
void TIM1_CC_IRQHandler     (void){}

VSF_CAL_WEAK(TIM2_IRQHandler)
void TIM2_IRQHandler        (void){}

VSF_CAL_WEAK(TIM3_IRQHandler)
void TIM3_IRQHandler        (void){}

VSF_CAL_WEAK(TIM4_IRQHandler)
void TIM4_IRQHandler        (void){}

VSF_CAL_WEAK(I2C1_EV_IRQHandler)
void I2C1_EV_IRQHandler     (void){}

VSF_CAL_WEAK(I2C1_ER_IRQHandler)
void I2C1_ER_IRQHandler     (void){}

VSF_CAL_WEAK(I2C2_EV_IRQHandler)
void I2C2_EV_IRQHandler     (void){}

VSF_CAL_WEAK(I2C2_ER_IRQHandler)
void I2C2_ER_IRQHandler     (void){}

VSF_CAL_WEAK(SPI1_IRQHandler)
void SPI1_IRQHandler        (void){}

VSF_CAL_WEAK(SPI2_IRQHandler)
void SPI2_IRQHandler        (void){}

VSF_CAL_WEAK(USART1_IRQHandler)
void USART1_IRQHandler      (void){}

VSF_CAL_WEAK(USART2_IRQHandler)
void USART2_IRQHandler      (void){}

VSF_CAL_WEAK(USART3_IRQHandler)
void USART3_IRQHandler      (void){}

VSF_CAL_WEAK(EXTI15_10_IRQHandler)
void EXTI15_10_IRQHandler   (void){}

VSF_CAL_WEAK(RTC_Alarm_IRQHandler)
void RTC_Alarm_IRQHandler   (void){}

VSF_CAL_WEAK(SWI0_IRQHandler)
void SWI0_IRQHandler        (void){}

VSF_CAL_WEAK(TIM8_BRK_TIM12_IRQHandler)
void TIM8_BRK_TIM12_IRQHandler(void){}

VSF_CAL_WEAK(TIM8_UP_TIM13_IRQHandler)
void TIM8_UP_TIM13_IRQHandler(void){}

VSF_CAL_WEAK(TIM8_TRG_COM_TIM14_IRQHandler)
void TIM8_TRG_COM_TIM14_IRQHandler(void){}

VSF_CAL_WEAK(TIM8_CC_IRQHandler)
void TIM8_CC_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Stream7_IRQHandler)
void DMA1_Stream7_IRQHandler(void){}

VSF_CAL_WEAK(FMC_IRQHandler)
void FMC_IRQHandler         (void){}

VSF_CAL_WEAK(SDMMC1_IRQHandler)
void SDMMC1_IRQHandler      (void){}

VSF_CAL_WEAK(TIM5_IRQHandler)
void TIM5_IRQHandler        (void){}

VSF_CAL_WEAK(SPI3_IRQHandler)
void SPI3_IRQHandler        (void){}

VSF_CAL_WEAK(UART4_IRQHandler)
void UART4_IRQHandler       (void){}

VSF_CAL_WEAK(UART5_IRQHandler)
void UART5_IRQHandler       (void){}

VSF_CAL_WEAK(TIM6_DAC_IRQHandler)
void TIM6_DAC_IRQHandler    (void){}

VSF_CAL_WEAK(TIM7_IRQHandler)
void TIM7_IRQHandler        (void){}

VSF_CAL_WEAK(DMA2_Stream0_IRQHandler)
void DMA2_Stream0_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Stream1_IRQHandler)
void DMA2_Stream1_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Stream2_IRQHandler)
void DMA2_Stream2_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Stream3_IRQHandler)
void DMA2_Stream3_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Stream4_IRQHandler)
void DMA2_Stream4_IRQHandler(void){}

VSF_CAL_WEAK(ETH_IRQHandler)
void ETH_IRQHandler         (void){}

VSF_CAL_WEAK(ETH_WKUP_IRQHandler)
void ETH_WKUP_IRQHandler    (void){}

VSF_CAL_WEAK(FDCAN_CAL_IRQHandler)
void FDCAN_CAL_IRQHandler   (void){}

VSF_CAL_WEAK(CM7_SEV_IRQHandler)
void CM7_SEV_IRQHandler     (void){}

VSF_CAL_WEAK(CM4_SEV_IRQHandler)
void CM4_SEV_IRQHandler     (void){}

VSF_CAL_WEAK(SWI1_IRQHandler)
void SWI1_IRQHandler        (void){}

VSF_CAL_WEAK(SWI2_IRQHandler)
void SWI2_IRQHandler        (void){}

VSF_CAL_WEAK(DMA2_Stream5_IRQHandler)
void DMA2_Stream5_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Stream6_IRQHandler)
void DMA2_Stream6_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Stream7_IRQHandler)
void DMA2_Stream7_IRQHandler(void){}

VSF_CAL_WEAK(USART6_IRQHandler)
void USART6_IRQHandler      (void){}

VSF_CAL_WEAK(I2C3_EV_IRQHandler)
void I2C3_EV_IRQHandler     (void){}

VSF_CAL_WEAK(I2C3_ER_IRQHandler)
void I2C3_ER_IRQHandler     (void){}

VSF_CAL_WEAK(OTG_HS_EP1_OUT_IRQHandler)
void OTG_HS_EP1_OUT_IRQHandler(void){}

VSF_CAL_WEAK(OTG_HS_EP1_IN_IRQHandler)
void OTG_HS_EP1_IN_IRQHandler(void){}

VSF_CAL_WEAK(OTG_HS_WKUP_IRQHandler)
void OTG_HS_WKUP_IRQHandler (void){}

VSF_CAL_WEAK(OTG_HS_IRQHandler)
void OTG_HS_IRQHandler      (void){}

VSF_CAL_WEAK(DCMI_IRQHandler)
void DCMI_IRQHandler        (void){}

VSF_CAL_WEAK(CRYP_IRQHandler)
void CRYP_IRQHandler        (void){}

VSF_CAL_WEAK(HASH_RNG_IRQHandler)
void HASH_RNG_IRQHandler    (void){}

VSF_CAL_WEAK(FPU_IRQHandler)
void FPU_IRQHandler         (void){}

VSF_CAL_WEAK(UART7_IRQHandler)
void UART7_IRQHandler       (void){}

VSF_CAL_WEAK(UART8_IRQHandler)
void UART8_IRQHandler       (void){}

VSF_CAL_WEAK(SPI4_IRQHandler)
void SPI4_IRQHandler        (void){}

VSF_CAL_WEAK(SPI5_IRQHandler)
void SPI5_IRQHandler        (void){}

VSF_CAL_WEAK(SPI6_IRQHandler)
void SPI6_IRQHandler        (void){}

VSF_CAL_WEAK(SAI1_IRQHandler)
void SAI1_IRQHandler        (void){}

VSF_CAL_WEAK(LTDC_IRQHandler)
void LTDC_IRQHandler        (void){}

VSF_CAL_WEAK(LTDC_ER_IRQHandler)
void LTDC_ER_IRQHandler     (void){}

VSF_CAL_WEAK(DMA2D_IRQHandler)
void DMA2D_IRQHandler       (void){}

VSF_CAL_WEAK(SAI2_IRQHandler)
void SAI2_IRQHandler        (void){}

VSF_CAL_WEAK(QUADSPI_IRQHandler)
void QUADSPI_IRQHandler     (void){}

VSF_CAL_WEAK(LPTIM1_IRQHandler)
void LPTIM1_IRQHandler      (void){}

VSF_CAL_WEAK(CEC_IRQHandler)
void CEC_IRQHandler         (void){}

VSF_CAL_WEAK(I2C4_EV_IRQHandler)
void I2C4_EV_IRQHandler     (void){}

VSF_CAL_WEAK(I2C4_ER_IRQHandler)
void I2C4_ER_IRQHandler     (void){}

VSF_CAL_WEAK(SPDIF_RX_IRQHandler)
void SPDIF_RX_IRQHandler    (void){}

VSF_CAL_WEAK(OTG_FS_EP1_OUT_IRQHandler)
void OTG_FS_EP1_OUT_IRQHandler(void){}

VSF_CAL_WEAK(OTG_FS_EP1_IN_IRQHandler)
void OTG_FS_EP1_IN_IRQHandler(void){}

VSF_CAL_WEAK(OTG_FS_WKUP_IRQHandler)
void OTG_FS_WKUP_IRQHandler (void){}

VSF_CAL_WEAK(OTG_FS_IRQHandler)
void OTG_FS_IRQHandler      (void){}

VSF_CAL_WEAK(DMAMUX1_OVR_IRQHandler)
void DMAMUX1_OVR_IRQHandler (void){}

VSF_CAL_WEAK(HRTIM1_Master_IRQHandler)
void HRTIM1_Master_IRQHandler(void){}

VSF_CAL_WEAK(HRTIM1_TIMA_IRQHandler)
void HRTIM1_TIMA_IRQHandler (void){}

VSF_CAL_WEAK(HRTIM1_TIMB_IRQHandler)
void HRTIM1_TIMB_IRQHandler (void){}

VSF_CAL_WEAK(HRTIM1_TIMC_IRQHandler)
void HRTIM1_TIMC_IRQHandler (void){}

VSF_CAL_WEAK(HRTIM1_TIMD_IRQHandler)
void HRTIM1_TIMD_IRQHandler (void){}

VSF_CAL_WEAK(HRTIM1_TIME_IRQHandler)
void HRTIM1_TIME_IRQHandler (void){}

VSF_CAL_WEAK(HRTIM1_FLT_IRQHandler)
void HRTIM1_FLT_IRQHandler  (void){}

VSF_CAL_WEAK(DFSDM1_FLT0_IRQHandler)
void DFSDM1_FLT0_IRQHandler (void){}

VSF_CAL_WEAK(DFSDM1_FLT1_IRQHandler)
void DFSDM1_FLT1_IRQHandler (void){}

VSF_CAL_WEAK(DFSDM1_FLT2_IRQHandler)
void DFSDM1_FLT2_IRQHandler (void){}

VSF_CAL_WEAK(DFSDM1_FLT3_IRQHandler)
void DFSDM1_FLT3_IRQHandler (void){}

VSF_CAL_WEAK(SAI3_IRQHandler)
void SAI3_IRQHandler        (void){}

VSF_CAL_WEAK(SWPMI1_IRQHandler)
void SWPMI1_IRQHandler      (void){}

VSF_CAL_WEAK(TIM15_IRQHandler)
void TIM15_IRQHandler       (void){}

VSF_CAL_WEAK(TIM16_IRQHandler)
void TIM16_IRQHandler       (void){}

VSF_CAL_WEAK(TIM17_IRQHandler)
void TIM17_IRQHandler       (void){}

VSF_CAL_WEAK(MDIOS_WKUP_IRQHandler)
void MDIOS_WKUP_IRQHandler  (void){}

VSF_CAL_WEAK(MDIOS_IRQHandler)
void MDIOS_IRQHandler       (void){}

VSF_CAL_WEAK(JPEG_IRQHandler)
void JPEG_IRQHandler        (void){}

VSF_CAL_WEAK(MDMA_IRQHandler)
void MDMA_IRQHandler        (void){}

VSF_CAL_WEAK(DSI_IRQHandler)
void DSI_IRQHandler         (void){}

VSF_CAL_WEAK(SDMMC2_IRQHandler)
void SDMMC2_IRQHandler      (void){}

VSF_CAL_WEAK(HSEM1_IRQHandler)
void HSEM1_IRQHandler       (void){}

VSF_CAL_WEAK(HSEM2_IRQHandler)
void HSEM2_IRQHandler       (void){}

VSF_CAL_WEAK(ADC3_IRQHandler)
void ADC3_IRQHandler        (void){}

VSF_CAL_WEAK(DMAMUX2_OVR_IRQHandler)
void DMAMUX2_OVR_IRQHandler (void){}

VSF_CAL_WEAK(BDMA_Channel0_IRQHandler)
void BDMA_Channel0_IRQHandler(void){}

VSF_CAL_WEAK(BDMA_Channel1_IRQHandler)
void BDMA_Channel1_IRQHandler(void){}

VSF_CAL_WEAK(BDMA_Channel2_IRQHandler)
void BDMA_Channel2_IRQHandler(void){}

VSF_CAL_WEAK(BDMA_Channel3_IRQHandler)
void BDMA_Channel3_IRQHandler(void){}

VSF_CAL_WEAK(BDMA_Channel4_IRQHandler)
void BDMA_Channel4_IRQHandler(void){}

VSF_CAL_WEAK(BDMA_Channel5_IRQHandler)
void BDMA_Channel5_IRQHandler(void){}

VSF_CAL_WEAK(BDMA_Channel6_IRQHandler)
void BDMA_Channel6_IRQHandler(void){}

VSF_CAL_WEAK(BDMA_Channel7_IRQHandler)
void BDMA_Channel7_IRQHandler(void){}

VSF_CAL_WEAK(COMP1_IRQHandler)
void COMP1_IRQHandler       (void){}

VSF_CAL_WEAK(LPTIM2_IRQHandler)
void LPTIM2_IRQHandler      (void){}

VSF_CAL_WEAK(LPTIM3_IRQHandler)
void LPTIM3_IRQHandler      (void){}

VSF_CAL_WEAK(LPTIM4_IRQHandler)
void LPTIM4_IRQHandler      (void){}

VSF_CAL_WEAK(LPTIM5_IRQHandler)
void LPTIM5_IRQHandler      (void){}

VSF_CAL_WEAK(LPUART1_IRQHandler)
void LPUART1_IRQHandler     (void){}

VSF_CAL_WEAK(WWDG_RST_IRQHandler)
void WWDG_RST_IRQHandler    (void){}

VSF_CAL_WEAK(CRS_IRQHandler)
void CRS_IRQHandler         (void){}

VSF_CAL_WEAK(ECC_IRQHandler)
void ECC_IRQHandler         (void){}

VSF_CAL_WEAK(SAI4_IRQHandler)
void SAI4_IRQHandler        (void){}

VSF_CAL_WEAK(SWI3_IRQHandler)
void SWI3_IRQHandler        (void){}

VSF_CAL_WEAK(HOLD_CORE_IRQHandler)
void HOLD_CORE_IRQHandler   (void){}

VSF_CAL_WEAK(WAKEUP_PIN_IRQHandler)
void WAKEUP_PIN_IRQHandler  (void){}

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

VSF_CAL_ROOT const pFunc __VECTOR_TABLE[240] __VECTOR_TABLE_ATTRIBUTE = {
    (pFunc)(&__INITIAL_SP),                   /*     Initial Stack Pointer */
    Reset_Handler,                            /*     Reset Handler */
    NMI_Handler,                              /* -14 NMI Handler */
    HardFault_Handler,                        /* -13 Hard Fault Handler */
    MemManage_Handler,                        /* -12 MPU Fault Handler */
    BusFault_Handler,                         /* -11 Bus Fault Handler */
    UsageFault_Handler,                       /* -10 Usage Fault Handler */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    SVC_Handler,                              /*  -5 SVCall Handler */
    DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
    0,                                        /*     Reserved */
    PendSV_Handler,                           /*  -2 PendSV Handler */
    SysTick_Handler,                          /*  -1 SysTick Handler */

    /* Interrupts */
    WWDG_IRQHandler,                          /*   0 Window WatchDog */
    PVD_AVD_IRQHandler,                       /*   1 PVD/AVD through EXTI line detection */
    TAMP_STAMP_IRQHandler,                    /*   2 Tamper and TimeStamp through the EXTI line */
    RTC_WKUP_IRQHandler,                      /*   3 RTC Wakeup through the EXTI line */
    FLASH_IRQHandler,                         /*   4 Flash */
    RCC_IRQHandler,                           /*   5 RCC */
    EXTI0_IRQHandler,                         /*   6 EXTI Line0 */
    EXTI1_IRQHandler,                         /*   7 EXTI Line1 */
    EXTI2_IRQHandler,                         /*   8 EXTI Line2 */
    EXTI3_IRQHandler,                         /*   9 EXTI Line3 */
    EXTI4_IRQHandler,                         /*  10 EXTI Line4 */
    DMA1_Stream0_IRQHandler,                  /*  11 DMA1 Stream0 */
    DMA1_Stream1_IRQHandler,                  /*  12 DMA1 Stream1 */
    DMA1_Stream2_IRQHandler,                  /*  13 DMA1 Stream2 */
    DMA1_Stream3_IRQHandler,                  /*  14 DMA1 Stream3 */
    DMA1_Stream4_IRQHandler,                  /*  15 DMA1 Stream4 */
    DMA1_Stream5_IRQHandler,                  /*  16 DMA1 Stream5 */
    DMA1_Stream6_IRQHandler,                  /*  17 DMA1 Stream6 */
    ADC_IRQHandler,                           /*  18 ADC1, ADC2 */
    FDCAN1_IT0_IRQHandler,                    /*  19 FDCAN1 interrupt line 0 */
    FDCAN2_IT0_IRQHandler,                    /*  20 FDCAN2 interrupt line 0 */
    FDCAN1_IT1_IRQHandler,                    /*  21 FDCAN1 interrupt line 1 */
    FDCAN2_IT1_IRQHandler,                    /*  22 FDCAN2 interrupt line 1 */
    EXTI9_5_IRQHandler,                       /*  23 EXTI Line[9:5]s */
    TIM1_BRK_IRQHandler,                      /*  24 TIM1 Break interrupt */
    TIM1_UP_IRQHandler,                       /*  25 TIM1 Update interrupt */
    TIM1_TRG_COM_IRQHandler,                  /*  26 TIM1 Trigger and Commutation interrupt */
    TIM1_CC_IRQHandler,                       /*  27 TIM1 Capture Compare */
    TIM2_IRQHandler,                          /*  28 TIM2 */
    TIM3_IRQHandler,                          /*  29 TIM3 */
    TIM4_IRQHandler,                          /*  30 TIM4 */
    I2C1_EV_IRQHandler,                       /*  31 I2C1 Event */
    I2C1_ER_IRQHandler,                       /*  32 I2C1 Error */
    I2C2_EV_IRQHandler,                       /*  33 I2C2 Event */
    I2C2_ER_IRQHandler,                       /*  34 I2C2 Error */
    SPI1_IRQHandler,                          /*  35 SPI1 */
    SPI2_IRQHandler,                          /*  36 SPI2 */
    USART1_IRQHandler,                        /*  37 USART1 */
    USART2_IRQHandler,                        /*  38 USART2 */
    USART3_IRQHandler,                        /*  39 USART3 */
    EXTI15_10_IRQHandler,                     /*  40 External Line[15:10] */
    RTC_Alarm_IRQHandler,                     /*  41 RTC Alarm (A and B) through EXTI Line */
    SWI0_IRQHandler,                          /*  42 SWI0 */
    TIM8_BRK_TIM12_IRQHandler,                /*  43 TIM8 Break and TIM12 */
    TIM8_UP_TIM13_IRQHandler,                 /*  44 TIM8 Update and TIM13 */
    TIM8_TRG_COM_TIM14_IRQHandler,            /*  45 TIM8 Trigger and Commutation and TIM14 */
    TIM8_CC_IRQHandler,                       /*  46 TIM8 Capture Compare */
    DMA1_Stream7_IRQHandler,                  /*  47 DMA1 Stream7 */
    FMC_IRQHandler,                           /*  48 FMC */
    SDMMC1_IRQHandler,                        /*  49 SDMMC1 */
    TIM5_IRQHandler,                          /*  50 TIM5 */
    SPI3_IRQHandler,                          /*  51 SPI3 */
    UART4_IRQHandler,                         /*  52 UART4 */
    UART5_IRQHandler,                         /*  53 UART5 */
    TIM6_DAC_IRQHandler,                      /*  54 TIM6 and DAC1&2 underrun errors */
    TIM7_IRQHandler,                          /*  55 TIM7 */
    DMA2_Stream0_IRQHandler,                  /*  56 DMA2 Stream0 */
    DMA2_Stream1_IRQHandler,                  /*  57 DMA2 Stream1 */
    DMA2_Stream2_IRQHandler,                  /*  58 DMA2 Stream2 */
    DMA2_Stream3_IRQHandler,                  /*  59 DMA2 Stream3 */
    DMA2_Stream4_IRQHandler,                  /*  60 DMA2 Stream4 */
    ETH_IRQHandler,                           /*  61 Ethernet */
    ETH_WKUP_IRQHandler,                      /*  62 Ethernet wakeup */
    FDCAN_CAL_IRQHandler,                     /*  63 FDCAN calibration unit interrupt */
    CM7_SEV_IRQHandler,                       /*  64 CM7 Send event interrupt for CM4 */
    CM4_SEV_IRQHandler,                       /*  65 CM4 Send event interrupt for CM7 */
    SWI1_IRQHandler,                          /*  66 SWI1 */
    SWI2_IRQHandler,                          /*  67 SWI2 */
    DMA2_Stream5_IRQHandler,                  /*  68 DMA2 Stream5 */
    DMA2_Stream6_IRQHandler,                  /*  69 DMA2 Stream6 */
    DMA2_Stream7_IRQHandler,                  /*  70 DMA2 Stream7 */
    USART6_IRQHandler,                        /*  71 USART6 */
    I2C3_EV_IRQHandler,                       /*  72 I2C3 event */
    I2C3_ER_IRQHandler,                       /*  73 I2C3 error */
    OTG_HS_EP1_OUT_IRQHandler,                /*  74 USB OTG HS End Point 1 Out */
    OTG_HS_EP1_IN_IRQHandler,                 /*  75 USB OTG HS End Point 1 In */
    OTG_HS_WKUP_IRQHandler,                   /*  76 USB OTG HS Wakeup through EXTI */
    OTG_HS_IRQHandler,                        /*  77 USB OTG HS */
    DCMI_IRQHandler,                          /*  78 DCMI */
    CRYP_IRQHandler,                          /*  79 CRYP */
    HASH_RNG_IRQHandler,                      /*  80 HASH and RNG */
    FPU_IRQHandler,                           /*  81 FPU */
    UART7_IRQHandler,                         /*  82 UART7 */
    UART8_IRQHandler,                         /*  83 UART8 */
    SPI4_IRQHandler,                          /*  84 SPI4 */
    SPI5_IRQHandler,                          /*  85 SPI5 */
    SPI6_IRQHandler,                          /*  86 SPI6 */
    SAI1_IRQHandler,                          /*  87 SAI */
    LTDC_IRQHandler,                          /*  88 LTDC */
    LTDC_ER_IRQHandler,                       /*  89 LTDC error */
    DMA2D_IRQHandler,                         /*  90 DMA2D */
    SAI2_IRQHandler,                          /*  91 SAI2 */
    QUADSPI_IRQHandler,                       /*  92 QuadSPI */
    LPTIM1_IRQHandler,                        /*  93 LP Timer1 */
    CEC_IRQHandler,                           /*  94 HDMI_CEC */
    I2C4_EV_IRQHandler,                       /*  95 I2C4 Event */
    I2C4_ER_IRQHandler,                       /*  96 I2C4 Error */
    SPDIF_RX_IRQHandler,                      /*  97 SPDIF_RX */
    OTG_FS_EP1_OUT_IRQHandler,                /*  98 USB OTG FS End Point 1 Out */
    OTG_FS_EP1_IN_IRQHandler,                 /*  99 USB OTG FS End Point 1 In */
    OTG_FS_WKUP_IRQHandler,                   /*  100 USB OTG FS Wakeup through EXTI*/
    OTG_FS_IRQHandler,                        /*  101 USB OTG FS */
    DMAMUX1_OVR_IRQHandler,                   /*  102 DMAMUX1 Overrun interrupt */
    HRTIM1_Master_IRQHandler,                 /*  103 HRTIM Master Timer global Interrupts */
    HRTIM1_TIMA_IRQHandler,                   /*  104 HRTIM Timer A global Interrupt */
    HRTIM1_TIMB_IRQHandler,                   /*  105 HRTIM Timer B global Interrupt */
    HRTIM1_TIMC_IRQHandler,                   /*  106 HRTIM Timer C global Interrupt */
    HRTIM1_TIMD_IRQHandler,                   /*  107 HRTIM Timer D global Interrupt */
    HRTIM1_TIME_IRQHandler,                   /*  108 HRTIM Timer E global Interrupt */
    HRTIM1_FLT_IRQHandler,                    /*  109 HRTIM Fault global Interrupt */
    DFSDM1_FLT0_IRQHandler,                   /*  110 DFSDM Filter0 Interrupt */
    DFSDM1_FLT1_IRQHandler,                   /*  111 DFSDM Filter1 Interrupt */
    DFSDM1_FLT2_IRQHandler,                   /*  112 DFSDM Filter2 Interrupt */
    DFSDM1_FLT3_IRQHandler,                   /*  113 DFSDM Filter3 Interrupt */
    SAI3_IRQHandler,                          /*  114 SAI3 */
    SWPMI1_IRQHandler,                        /*  115 Serial Wire Interface 1 */
    TIM15_IRQHandler,                         /*  116 TIM15 */
    TIM16_IRQHandler,                         /*  117 TIM16 */
    TIM17_IRQHandler,                         /*  118 TIM17 */
    MDIOS_WKUP_IRQHandler,                    /*  119 MDIOS Wakeup Interrupt */
    MDIOS_IRQHandler,                         /*  120 MDIOS global Interrupt */
    JPEG_IRQHandler,                          /*  121 JPEG */
    MDMA_IRQHandler,                          /*  122 MDMA */
    DSI_IRQHandler,                           /*  123 DSI */
    SDMMC2_IRQHandler,                        /*  124 SDMMC2 */
    HSEM1_IRQHandler,                         /*  125 HSEM1 */
    HSEM2_IRQHandler,                         /*  126 HSEM2 */
    ADC3_IRQHandler,                          /*  127 ADC3 */
    DMAMUX2_OVR_IRQHandler,                   /*  128 DMAMUX Overrun interrupt */
    BDMA_Channel0_IRQHandler,                 /*  129 BDMA Channel 0 global Interrupt */
    BDMA_Channel1_IRQHandler,                 /*  130 BDMA Channel 1 global Interrupt */
    BDMA_Channel2_IRQHandler,                 /*  131 BDMA Channel 2 global Interrupt */
    BDMA_Channel3_IRQHandler,                 /*  132 BDMA Channel 3 global Interrupt */
    BDMA_Channel4_IRQHandler,                 /*  133 BDMA Channel 4 global Interrupt */
    BDMA_Channel5_IRQHandler,                 /*  134 BDMA Channel 5 global Interrupt */
    BDMA_Channel6_IRQHandler,                 /*  135 BDMA Channel 6 global Interrupt */
    BDMA_Channel7_IRQHandler,                 /*  136 BDMA Channel 7 global Interrupt */
    COMP1_IRQHandler,                         /*  137 COMP1 */
    LPTIM2_IRQHandler,                        /*  138 LPTIM2 */
    LPTIM3_IRQHandler,                        /*  139 LPTIM3 */
    LPTIM4_IRQHandler,                        /*  140 LPTIM4 */
    LPTIM5_IRQHandler,                        /*  141 LPTIM5 */
    LPUART1_IRQHandler,                       /*  142 LPUART1 */
    WWDG_RST_IRQHandler,                      /*  143 Window Watchdog reset interrupt */
    CRS_IRQHandler,                           /*  144 CSR */
    ECC_IRQHandler,                           /*  145 ECC */
    SAI4_IRQHandler,                          /*  146 SAI4 */
    SWI3_IRQHandler,                          /*  147 SWI3 */
    HOLD_CORE_IRQHandler,                     /*  148 Hold core */
    WAKEUP_PIN_IRQHandler,                    /*  149 Interrupt for all 6 wake-up pins */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

VSF_CAL_WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{
    extern void SystemInit(void);
    SystemInit();
    SCB_EnableICache();
    SCB_EnableDCache();
    SCB->CACR |= SCB_CACR_FORCEWT_Msk;
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    __set_MSP((uintptr_t)&__INITIAL_SP);
    //! enable FPU before vsf_hal_pre_startup_init, in case vsf_hal_pre_startup_init uses FPU
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U));           /* enable CP11 Full Access */

    vsf_hal_pre_startup_init();

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
