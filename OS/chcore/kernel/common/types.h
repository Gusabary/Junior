/*
 * Copyright (c) 2020 Institute of Parallel And Distributed Systems (IPADS), Shanghai Jiao Tong University (SJTU)
 * OS-Lab-2020 (i.e., ChCore) is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *   http://license.coscl.org.cn/MulanPSL
 *   THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 *   PURPOSE.
 *   See the Mulan PSL v1 for more details.
 */

#pragma once

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef long long s64;
typedef int s32;
typedef short s16;
typedef signed char s8;

#ifdef CHCORE

#define NULL ((void *)0)

#else
#include <stdlib.h>
#endif

typedef char bool;
#define true (1)
#define false (0)
typedef u64 paddr_t;
typedef u64 vaddr_t;

typedef u64 atomic_cnt;

/* Different platform may have different cacheline size and may have some features like prefetch */
#define CACHELINE_SZ 64
#define r_align(n, r)        (((n) + (r) - 1) & -(r))
#define cache_align(n)       r_align(n , CACHELINE_SZ)
#define pad_to_cache_line(n) (cache_align(n) - (n))
