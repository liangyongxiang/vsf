set(QEMU_FAKE_SOC_BSP_ROOT
    ${VSF_SRC_PATH}/hal/driver/arm/mps2/common/V2M-MPS2_CMx_BSP/1.7.1/Device/CMSDK_CM7
)

vsf_add_sources(
    __common.c
    ${QEMU_FAKE_SOC_BSP_ROOT}/Source/system_CMSDK_CM7.c
)

add_subdirectory(debug_uart)
