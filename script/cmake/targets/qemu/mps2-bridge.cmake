set(VSF_HAL_CHIP_VENDOR     qemu)
set(VSF_HAL_CHIP_SERIES     mps2-bridge)
set(VSF_HAL_CHIP_NAME       mps2_bridge)

set(VSF_TARGET_DEFINITIONS
    "__QEMU__"
    "__QEMU_MPS2_BRIDGE__"
    "__MPS2__"
    "CMSDK_CM7"
    "VSF_USE_LINUX=DISABLED"
    "VSF_USE_POSIX=DISABLED"

    ${VSF_TARGET_DEFINITIONS}
)

include(${VSF_CMAKE_ROOT}/targets/arm/__cortex_m7.cmake)

set(VSF_TARGET_PATH ${VSF_SRC_PATH}/hal/driver/${VSF_HAL_CHIP_VENDOR}/${VSF_HAL_CHIP_SERIES})
list(APPEND VSF_TARGET_INCLUDE_DIRECTORIES
    ${VSF_TARGET_PATH}
    ${VSF_SRC_PATH}/hal/driver/arm/mps2/common/V2M-MPS2_CMx_BSP/1.7.1/Device/CMSDK_CM7/Include
)
