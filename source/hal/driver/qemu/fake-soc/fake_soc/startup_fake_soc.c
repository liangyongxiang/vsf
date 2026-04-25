/******************************************************************************
 * @file     startup_fake_soc.c
 * @brief    Startup file for the QEMU fake SoC CMSDK-CM7 skeleton
 * @version  V1.0.0
 ******************************************************************************/

#include "./device.h"

typedef void(*pFunc)(void);

extern uint32_t __INITIAL_SP;
extern __NO_RETURN void __PROGRAM_START(void);

void __NO_RETURN Default_Handler(void);
void __NO_RETURN Reset_Handler(void);

#define WEAK_ISR(__NAME, ...)                                                   \
    VSF_CAL_WEAK(__NAME)                                                        \
    void __NAME(void) { __VA_ARGS__ }

WEAK_ISR(NMI_Handler)
WEAK_ISR(HardFault_Handler, while (1);)
WEAK_ISR(MemManage_Handler, while (1);)
WEAK_ISR(BusFault_Handler, while (1);)
WEAK_ISR(UsageFault_Handler, while (1);)
WEAK_ISR(SVC_Handler)
WEAK_ISR(DebugMon_Handler, while (1);)
WEAK_ISR(PendSV_Handler)
WEAK_ISR(SysTick_Handler)

WEAK_ISR(UART0RX_Handler)
WEAK_ISR(UART0TX_Handler)
WEAK_ISR(UART1RX_Handler)
WEAK_ISR(UART1TX_Handler)
WEAK_ISR(UART2RX_Handler)
WEAK_ISR(UART2TX_Handler)
WEAK_ISR(GPIO0ALL_Handler)
WEAK_ISR(GPIO1ALL_Handler)
WEAK_ISR(TIMER0_Handler)
WEAK_ISR(TIMER1_Handler)
WEAK_ISR(DUALTIMER_Handler)
WEAK_ISR(SPI_0_1_Handler)
WEAK_ISR(UART_0_1_2_OVF_Handler)
WEAK_ISR(ETHERNET_Handler)
WEAK_ISR(I2S_Handler)
WEAK_ISR(TOUCHSCREEN_Handler)
WEAK_ISR(GPIO2_Handler)
WEAK_ISR(GPIO3_Handler)
WEAK_ISR(UART3RX_Handler)
WEAK_ISR(UART3TX_Handler)
WEAK_ISR(UART4RX_Handler)
WEAK_ISR(UART4TX_Handler)
WEAK_ISR(SPI_2_Handler)
WEAK_ISR(SPI_3_4_Handler)

#define __DECLARE_SWI_IRQ_HANDLER(__N, __NULL)                                  \
    WEAK_ISR(SWI##__N##_IRQHandler)

VSF_MREPEAT(VSF_DEV_SWI_NUM, __DECLARE_SWI_IRQ_HANDLER, NULL)

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
    (pFunc)(&__INITIAL_SP),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,

    UART0RX_Handler,
    UART0TX_Handler,
    UART1RX_Handler,
    UART1TX_Handler,
    UART2RX_Handler,
    UART2TX_Handler,
    GPIO0ALL_Handler,
    GPIO1ALL_Handler,
    TIMER0_Handler,
    TIMER1_Handler,
    DUALTIMER_Handler,
    SPI_0_1_Handler,
    UART_0_1_2_OVF_Handler,
    ETHERNET_Handler,
    I2S_Handler,
    TOUCHSCREEN_Handler,
    GPIO2_Handler,
    GPIO3_Handler,
    UART3RX_Handler,
    UART3TX_Handler,
    UART4RX_Handler,
    UART4TX_Handler,
    SPI_2_Handler,
    SPI_3_4_Handler,
    SWI0_IRQHandler,
    SWI1_IRQHandler,
    SWI2_IRQHandler,
    SWI3_IRQHandler,
    SWI4_IRQHandler,
    SWI5_IRQHandler,
    SWI6_IRQHandler,
};

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

VSF_CAL_WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{
}

void Reset_Handler(void)
{
    __set_MSP((uintptr_t)&__INITIAL_SP);
    vsf_hal_pre_startup_init();
    SCB->CPACR |= ((3U << 10U * 2U) | (3U << 11U * 2U));

#ifdef UNALIGNED_SUPPORT_DISABLE
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

    __PROGRAM_START();
}
