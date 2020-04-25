#pragma once

#include <lib/type.h>

#define SYS_putc                0
#define SYS_getc                1
#define SYS_yield               2
#define SYS_exit                3
#define SYS_sleep               4
#define SYS_create_pmo          5
#define SYS_map_pmo             6
#define SYS_create_thread       7
#define SYS_create_process    8
#define SYS_register_server     9
#define SYS_register_client     10
#define SYS_ipc_call            12
#define SYS_ipc_return          13
#define SYS_cap_copy_to         15
#define SYS_cap_copy_from       16
#define SYS_unmap_pmo           17
#define SYS_set_affinity	18
#define SYS_get_affinity	19
#define SYS_create_device_pmo   20
/*
 * Lab 4
 * Add syscall
 */
#define SYS_get_cpu_id 50
#define SYS_ipc_reg_call            51

#define SYS_create_pmos         101
#define SYS_map_pmos            102
#define SYS_write_pmo           103
#define SYS_read_pmo            104
#define SYS_transfer_caps       105

#define SYS_handle_brk		201

/* TEMP */
#define SYS_fs_load_cpio        253
#define SYS_debug               255

int usys_fs_load_cpio(u64 vaddr);
/* TEMP END */


void usys_putc(char ch);
void usys_exit(int ret);
int usys_create_pmo(u64 size, u64 type);
int usys_map_pmo(u64 process_cap, u64 pmo_cap, u64 addr, u64 perm);
u64 usys_handle_brk(u64 addr);
/* lab3 syscalls finished */
