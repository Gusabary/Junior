cmake_minimum_required(VERSION 3.14)

project(user C ASM)

set(CROSS_COMPILE "aarch64-linux-gnu-")

set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
set(CMAKE_ASM_COMPILER "${CROSS_COMPILE}gcc")

set(C_COMPILER_NOSTDINC "-nostdinc -nostdlib -nostartfiles")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    ${C_COMPILER_NOSTDINC}")

message(${CMAKE_C_FLAGS})

set(LINK_FLAGS "-static")

# add_subdirectory(lab3)
add_subdirectory(lab4)
include_directories(".")
include_directories("lib")

# A special one: the first user process created by the kernel
file(
    GLOB
        init_source_files
        "apps/init.c"
        "apps/init_main.c"
        "lib/*.c"
)
add_executable(init.bin ${init_source_files})

file(
    GLOB
        init_test_files
        "apps/init.c"
        "apps/init_test.c"
        "lib/*.c"
)
add_executable(init_test.bin ${init_test_files})

add_subdirectory(tmpfs)
