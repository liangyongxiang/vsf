set(VSF_HAL_CHIP_VENDOR     qemu)
set(VSF_HAL_CHIP_SERIES     fake-soc)
set(VSF_HAL_CHIP_NAME       fake_soc)

set(VSF_TARGET_DEFINITIONS
    "__QEMU__"
    "__FAKE_SOC__"
    "__QEMU_FAKE_SOC__"
    "CMSDK_CM7"
    "VSF_USE_LINUX=DISABLED"
    "VSF_USE_POSIX=DISABLED"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m7.cmake)

set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${VSF_TARGET_PATH}
    ${VSF_TARGET_PATH}/common
    ${VSF_TARGET_PATH}/fake_soc
    ${VSF_SRC_PATH}/hal/driver/arm/mps2/common/V2M-MPS2_CMx_BSP/1.7.1/Device/CMSDK_CM7/Include
)
