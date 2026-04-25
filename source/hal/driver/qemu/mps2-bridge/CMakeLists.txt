set(QEMU_MPS2_BRIDGE_ROOT
    ${VSF_SRC_PATH}/hal/driver/arm/mps2
)
set(QEMU_MPS2_BRIDGE_BSP_ROOT
    ${QEMU_MPS2_BRIDGE_ROOT}/common/V2M-MPS2_CMx_BSP/1.7.1/Device/CMSDK_CM7
)

vsf_add_include_directories(
    ${QEMU_MPS2_BRIDGE_BSP_ROOT}/Include
)

vsf_add_sources(
    ${QEMU_MPS2_BRIDGE_ROOT}/common/__common.c
    ${QEMU_MPS2_BRIDGE_ROOT}/common/debug_uart/debug_uart.c
    ${QEMU_MPS2_BRIDGE_ROOT}/CMSDK_CM7/driver.c
    ${QEMU_MPS2_BRIDGE_ROOT}/CMSDK_CM7/startup_ARMCM7.c
    ${QEMU_MPS2_BRIDGE_BSP_ROOT}/Source/system_CMSDK_CM7.c
)
