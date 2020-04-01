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

#include "buddy_struct.h"

void init_buddy(struct global_mem *zone, struct page *start_page,
		vaddr_t start_addr, u64 page_num);

struct page *buddy_get_pages(struct global_mem *zone, u64 order);
void buddy_free_pages(struct global_mem *zone, struct page *page);

void *page_to_virt(struct global_mem *zone, struct page *page);
struct page *virt_to_page(struct global_mem *zone, void* ptr);
