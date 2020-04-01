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

#include <common/types.h>
#include <common/util.h>
#include <common/macro.h>
#include <common/kprint.h>


#include "buddy.h"


/*
 * local functions or types
 */

enum pageflags {
	PG_head,
	PG_buddy,
};

static inline void set_page_order_buddy(struct page *page, u64 order)
{
	page->order = order;
	page->flags |= (1UL << PG_buddy);
}

static inline void clear_page_order_buddy(struct page *page)
{
	page->order = 0;
	page->flags &= ~(1UL << PG_buddy);
}

static inline u64 get_order(struct page *page)
{
	return page->order;
}

static inline u64 get_page_idx(struct global_mem *zone, struct page *page)
{
	return (page - zone->first_page);
}

/*
 * return the start page (metadata) of the only one buddy.
 * e.g., page 0 and page 1 are buddies, page (0, 1) and page (2, 3) are buddies
 */
static inline struct page *get_buddy_page(struct global_mem *zone,
					  struct page *page,
					  u64 order)
{
	u64 page_idx;
	u64 buddy_idx;

	page_idx = get_page_idx(zone, page);
	/* find buddy index (to merge with) */
	buddy_idx = (page_idx ^ (1 << order));

	return zone->first_page + buddy_idx;
}

/* return the start page (metadata) after "merging" the page and its buddy */
static inline struct page *get_merge_page(struct global_mem *zone,
					  struct page *page,
					  u64 order)
{
	u64 page_idx;
	u64 merge_idx;

	page_idx  = get_page_idx(zone, page);
	merge_idx = page_idx & ~(1 << order);

	return zone->first_page + merge_idx;
}

static void split(struct global_mem *zone, struct page *page,
		   u64 low_order, u64 high_order,
		   struct free_list *list)
{
	u64 size;

	size = (1U << high_order);
	/* keep splitting */
	while (high_order > low_order) {
		list--;
		high_order--;
		size >>= 1;
		/* put into the corresponding free list */
		list_add(&page[size].list_node, &list->list_head);
		list->nr_free++;
		// set page order
		set_page_order_buddy(&page[size], high_order);
	}
}
/*
 * __aloc_page: get free page from buddy system(called by buddy_get_pages)
 * 
 * clear_page_order_buddy(): clear page metadata
 * 
 * Hint: Find the corresonding free_list which can allocate 1<<order
 * continuous pages and don't forget to split the list node after allocation   
 * 
 */
static struct page *__alloc_page(struct global_mem *zone, u64 order)
{
	//lab2
	if (order > BUDDY_MAX_ORDER) {
		return NULL;
	}

	struct list_head *free_node;
	struct free_list *list = zone->free_lists;
	u64 free_order = order;
	for (; free_order <= BUDDY_MAX_ORDER; free_order++)
	{
		if (list[free_order].nr_free > 0) {
			// have free page
			free_node = list[free_order].list_head.next;
			list_del(free_node);
			list[free_order].nr_free--;
			break;
		}
	}

	struct page *free_page = (struct page *)free_node;
	split(zone, free_page, order, free_order, &list[free_order]);
	// allocated page's flag should be cleared, otherwise it may be merged possibly
	clear_page_order_buddy(free_page);
	free_page->order = order;
	return free_page;
}

/* check whether the buddy can be merged */
static inline bool check_buddy(struct page *page, u64 order)
{
	return (page->flags & (1UL << PG_buddy))
		&& (page->order == order);
}

/*
 * exported functions
 */

void init_buddy(struct global_mem *zone, struct page *first_page,
		vaddr_t first_page_vaddr, u64 page_num)
{
	u64 i;
	struct page *page;
	struct free_list *list;


	/* init global memory metadata */
	zone->page_num = page_num;
	zone->page_size = BUDDY_PAGE_SIZE;
	zone->first_page = first_page;
	zone->start_addr = first_page_vaddr;
	zone->end_addr = zone->start_addr + page_num * BUDDY_PAGE_SIZE;

	/* init each free list (different orders) */
	for (i = 0; i < BUDDY_MAX_ORDER; i++) {
	        list = zone->free_lists + i;
		init_list_head(&list->list_head);
		list->nr_free = 0;
	}

	/* zero the metadata area (struct page for each page) */
	memset((char*)first_page, 0, page_num * sizeof(struct page));

	/* init the metadata for each page */
	for (i = 0; i < page_num; i++) {
		/* Currently, 0~img_end is reserved, however, if we want to use them, we should reserve MP_BOOT_ADDR */
		page = zone->first_page + i;
		init_list_head(&page->list_node);
		buddy_free_pages(zone, page);
	}

	kdebug("mm: finish initing buddy memory system.\n");
}
/*
 * budd_get_pages: get 1<<order continuous pages from buddy system
 * 
 * Hint: invoke __alloc_page to get free page. If order is larger than 
 * BUDDY_MAX_ORDER, return null.
 */
struct page *buddy_get_pages(struct global_mem *zone, u64 order)
{
	//lab2
	struct page *page = __alloc_page(zone, order);

	return page;
}

/*
 * buddy_free_pages: give back the pages to buddy system
 * 
 * clear_page_order_buddy(): clear page metadata
 * set_page_order_buddy(): set page metadata
 * 
 * Hint: you can use the get_order() to get corresponding order
 * before give back pages into buddy system. What's more, check whether 
 * given page can be merged with the corresponding buddy page
 * (use get_buddy_page). If page can be merged, update page after
 * merging (get_merge_page).
*/
void buddy_free_pages(struct global_mem *zone, struct page *page)
{
	//lab2
	u64 order = get_order(page);
	struct page *buddy_page = get_buddy_page(zone, page, order);
	struct page *merge_page = page;

	while (order < BUDDY_MAX_ORDER - 1 && check_buddy(buddy_page, order)) {
		// can be merged
		merge_page = get_merge_page(zone, page, order);
		
		// first operate on list and then clear page
		list_del(&buddy_page->list_node);
		zone->free_lists[buddy_page->order].nr_free--;
		clear_page_order_buddy(buddy_page);
		
		order++;
		page = merge_page;
		buddy_page = get_buddy_page(zone, page, order);
	}

	list_add(&merge_page->list_node, &zone->free_lists[order].list_head);
	zone->free_lists[order].nr_free++;
	set_page_order_buddy(merge_page, order);
}

void *page_to_virt(struct global_mem *zone, struct page *page)
{
	u64 page_idx;
	u64 address;

	if (page == NULL)
		return NULL;

	page_idx = get_page_idx(zone, page);
	address = zone->start_addr + page_idx * BUDDY_PAGE_SIZE;

	return (void *)address;
}

struct page *virt_to_page(struct global_mem *zone, void *ptr)
{
	u64 page_idx;
	struct page *page;
	u64 address;

	address = (u64)ptr;

	if ((address < zone->start_addr) || (address > zone->end_addr)) {
		kinfo("[BUG] start_addr=0x%lx, end_addr=0x%lx, address=0x%lx\n",
		       zone->start_addr, zone->end_addr, address);
		BUG_ON(1);
		return NULL;
	}
	page_idx = (address - zone->start_addr) >> BUDDY_PAGE_SHIFT;

	page = zone->first_page + page_idx;
	return page;
}
