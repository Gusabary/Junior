cmake_minimum_required(VERSION 3.14)

add_library(${PROJECT_NAME}-arch process
    head.S
    main.c
    monitor.c
    tools.S
    common/uart.c
    common/printk.c
    common/radix.c
)
