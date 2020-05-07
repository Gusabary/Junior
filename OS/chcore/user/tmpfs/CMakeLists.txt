cmake_minimum_required(VERSION 3.11)

include_directories("../lib")

file(
    GLOB
        tmpfs_source_files
        "tmpfs_main.c"
        "tmpfs_server.c"
        "tmpfs.c"
        "../lib/*.c"
)

add_executable(tmpfs.srv
    ${tmpfs_source_files}
)

file(
    GLOB
        tmpfs_test_files
        "tmpfs_test.c"
        "tmpfs_server.c"
        "tmpfs.c"
        "../lib/*.c"
)

add_executable(fs_test.bin
    ${tmpfs_test_files}
)

