cmake_minimum_required(VERSION 3.14)

set(INIT_PATH       "${BOOTLOADER_PATH}")
set(LIB_PATH        "${BOOTLOADER_PATH}/print")

file(
    GLOB
        tmpfiles
        "${INIT_PATH}/*.c"
        "${INIT_PATH}/*.S"
        "${LIB_PATH}/*.c"
        "${LIB_PATH}/*.S"
)

list(APPEND files ${tmpfiles})
