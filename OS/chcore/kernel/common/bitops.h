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
#include <common/macro.h>

#define BITS_PER_BYTE		8
#define BITS_PER_LONG		(sizeof(unsigned long) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_LONG)


static inline void set_bit(unsigned int nr, volatile unsigned long *p)
{
	unsigned nlongs = nr / BITS_PER_LONG;
	unsigned ilongs = nr % BITS_PER_LONG;
	p[nlongs] |= 1UL << ilongs;
}

static inline int get_bit(unsigned int nr, volatile unsigned long *p)
{
	unsigned nlongs = nr / BITS_PER_LONG;
	unsigned ilongs = nr % BITS_PER_LONG;
	return (p[nlongs] >> ilongs) & 0x1;
}

static inline void clear_bit(unsigned int nr, volatile unsigned long *p)
{
	unsigned nlongs = nr / BITS_PER_LONG;
	unsigned ilongs = nr % BITS_PER_LONG;
	p[nlongs] &= ~(1UL << ilongs);
}

static inline int ctzl(unsigned long x)
{
	return x == 0 ? sizeof(x) : __builtin_ctzl(x);
}

/* Borrowed from linux/lib/find_bit.c */
static inline int __find_next_bit(unsigned long *p, unsigned long size,
				  unsigned long start, unsigned long invert)
{
	unsigned long tmp;

	if (unlikely(size <= start))
		return size;
	tmp = p[start / BITS_PER_LONG] ^ invert;
	tmp &= ~((1UL << (start % BITS_PER_LONG)) - 1);
	start = ROUND_DOWN(start, BITS_PER_LONG);

	while (!tmp) {
		start += BITS_PER_LONG;
		if (start >= size)
			return size;
		tmp = p[start / BITS_PER_LONG] ^ invert;
	}

	return MIN(start + ctzl(tmp), size);
}

static inline int find_next_zero_bit(unsigned long *p, unsigned long size,
				     unsigned long start)
{
	return __find_next_bit(p, size, start, ~((unsigned long)0));
}

static inline int find_next_bit(unsigned long *p, unsigned long size,
				unsigned long start)
{
	return __find_next_bit(p, size, start, 0);
}

#define for_each_set_bit(pos, addr, size)		\
	for ((pos) = find_next_bit((addr), (size), 0);	\
	     (pos) < (size);				\
             (pos) = find_next_bit((addr), (size), (pos) + 1))
