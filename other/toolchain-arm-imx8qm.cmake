# Platform defines.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CROSSCOMPILING 1)
set(DE_CPU "DE_CPU_ARM")

set(CROSS_COMPILE_ENV "/home/software/Linux/freescale/ToolChain/L4.19.35-1.1.0_RC3_for_imx8/sysroots")
set(ROOTFS "${CROSS_COMPILE_ENV}/aarch64-poky-linux")
set(CMAKE_SYSROOT "${ROOTFS}")

# Toolchain/compiler base.
set(CROSS_COMPILE "${CROSS_COMPILE_ENV}/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-" CACHE STRING "Cross compiler prefix")
set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")

set(CMAKE_FIND_ROOT_PATH
        "${ROOTFS}/lib"
        "${ROOTFS}/usr/lib"
        "${ROOTFS}/usr/lib/aarch64-poky-linux/8.3.0"
        )

message(CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH}")

# Search libs and include files (but not programs) from toolchain dir.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY FIRST)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

add_definitions(-mtune=cortex-a53)
