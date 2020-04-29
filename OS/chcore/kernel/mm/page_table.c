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

#ifdef CHCORE
#include <common/util.h>
#include <common/kmalloc.h>
#endif
#include <common/vars.h>
#include <common/macro.h>
#include <common/types.h>
#include <common/errno.h>
#include <common/printk.h>
#include <common/mm.h>
#include <common/mmu.h>

#include "page_table.h"

/* Page_table.c: Use simple impl for debugging now. */

extern void set_ttbr0_el1(paddr_t);
extern void flush_tlb(void);

void set_page_table(paddr_t pgtbl)
{
	set_ttbr0_el1(pgtbl);
}

#define USER_PTE 0
#define KERNEL_PTE 1
/*
 * the 3rd arg means the kind of PTE.
 */
static int set_pte_flags(pte_t *entry, vmr_prop_t flags, int kind)
{
	if (flags & VMR_WRITE)
		entry->l3_page.AP = ARM64_MMU_ATTR_PAGE_AP_HIGH_RW_EL0_RW;
	else
		entry->l3_page.AP = ARM64_MMU_ATTR_PAGE_AP_HIGH_RO_EL0_RO;

	// FIXME: l1, l2 and l3's UXNs are in different bit position
	if (flags & VMR_EXEC)
		entry->l3_page.UXN = ARM64_MMU_ATTR_PAGE_UX;
	else
		entry->l3_page.UXN = ARM64_MMU_ATTR_PAGE_UXN;

	// EL1 cannot directly execute EL0 accessiable region.
	entry->l3_page.PXN = ARM64_MMU_ATTR_PAGE_PXN;
	entry->l3_page.AF  = ARM64_MMU_ATTR_PAGE_AF_ACCESSED;

	// not global
	//entry->l3_page.nG = 1;
	// inner sharable
	entry->l3_page.SH = INNER_SHAREABLE;
	// memory type
	entry->l3_page.attr_index = NORMAL_MEMORY;

	return 0;
}

#define GET_PADDR_IN_PTE(entry) \
	(((u64)entry->table.next_table_addr) << PAGE_SHIFT)
#define GET_NEXT_PTP(entry) phys_to_virt(GET_PADDR_IN_PTE(entry))

#define NORMAL_PTP (0)
#define BLOCK_PTP  (1)

/*
 * Find next page table page for the "va".
 *
 * cur_ptp: current page table page
 * level:   current ptp level
 *
 * next_ptp: returns "next_ptp"
 * pte     : returns "pte" (points to next_ptp) in "cur_ptp"
 *
 * alloc: if true, allocate a ptp when missing
 *
 */
static int get_next_ptp(ptp_t *cur_ptp, u32 level, vaddr_t va,
			ptp_t **next_ptp, pte_t **pte, bool alloc)
{
	u32 index = 0;
	pte_t *entry;

	if (cur_ptp == NULL)
		return -ENOMAPPING;

	switch (level) {
	case 0:
		index = GET_L0_INDEX(va);
		break;
	case 1:
		index = GET_L1_INDEX(va);
		break;
	case 2:
		index = GET_L2_INDEX(va);
		break;
	case 3:
		index = GET_L3_INDEX(va);
		break;
	default:
		BUG_ON(1);
	}

	entry = &(cur_ptp->ent[index]);
	if (IS_PTE_INVALID(entry->pte)) {
		if (alloc == false) {
			// if called from query or unmap, alloc shoule be false (const)
			// if called from map, alloc should be true (create if absent)
			return -ENOMAPPING;
		}
		else {
			/* alloc a new page table page */
			ptp_t *new_ptp;
			paddr_t new_ptp_paddr;
			pte_t new_pte_val;

			/* alloc a single physical page as a new page table page */
			new_ptp = get_pages(0);
			BUG_ON(new_ptp == NULL);
			memset((void *)new_ptp, 0, PAGE_SIZE);
			new_ptp_paddr = virt_to_phys((vaddr_t)new_ptp);

			new_pte_val.pte = 0;
			new_pte_val.table.is_valid = 1;
			new_pte_val.table.is_table = 1;
			new_pte_val.table.next_table_addr
				= new_ptp_paddr >> PAGE_SHIFT;

			/* same effect as: cur_ptp->ent[index] = new_pte_val; */
			entry->pte = new_pte_val.pte;
		}
	}

	*next_ptp = (ptp_t *)GET_NEXT_PTP(entry);
	*pte = entry;
	if (IS_PTE_TABLE(entry->pte))
		return NORMAL_PTP;
	else
		return BLOCK_PTP;
}

/*
 * Translate a va to pa, and get its pte for the flags
 */
/*
 * query_in_pgtbl: translate virtual address to physical 
 * address and return the corresponding page table entry
 * 
 * pgtbl @ ptr for the first level page table(pgd) virtual address
 * va @ query virtual address
 * pa @ return physical address
 * entry @ return page table entry
 * 
 * Hint: check the return value of get_next_ptp, if ret == BLOCK_PTP
 * return the pa and block entry immediately
 */
int query_in_pgtbl(vaddr_t *pgtbl, vaddr_t va, paddr_t *pa, pte_t **entry)
{
	//lab2
	ptp_t *cur_pgtbl = (ptp_t *)pgtbl;
	int level = 0;
	ptp_t *next_ptp;
	pte_t *pte;
	int rc;

	while (level <= 3) {
		rc = get_next_ptp(cur_pgtbl, level, va, &next_ptp, &pte, false);
		
		if (rc == -ENOMAPPING) {
			return rc;
		}
		
		if (rc == BLOCK_PTP || (rc == NORMAL_PTP && level == 3)) {
			*pa = virt_to_phys(next_ptp);
			*entry = pte;
			return 0;
		}

		// now rc is NORMAL_PTP, continue to walk the page table
		cur_pgtbl = next_ptp;
		level++;
	}

	// panic
	return -ENOMAPPING;
}

/*
 * map_range_in_pgtbl: map the virtual address [va:va+size] to 
 * physical address[pa:pa+size] in given pgtbl
 *
 * pgtbl @ ptr for the first level page table(pgd) virtual address
 * va @ start virtual address
 * pa @ start physical address
 * len @ mapping size
 * flags @ corresponding attribution bit
 *
 * Hint: In this function you should first invoke the get_next_ptp()
 * to get the each level page table entries. Read type pte_t carefully
 * and it is convenient for you to call set_pte_flags to set the page
 * permission bit. Don't forget to call flush_tlb at the end of this function 
 */
int map_range_in_pgtbl(vaddr_t *pgtbl, vaddr_t va, paddr_t pa,
		       size_t len, vmr_prop_t flags)
{
	//lab2: 
	ptp_t *next_ptp;
	pte_t *pte;
	int rc;
	u64 block_size;
	int final_level;

	if (flags & VMR_BLK_1G) {
		block_size = ARM64_MMU_L1_BLOCK_SIZE;
		final_level = 0;
	}
	else if (flags & VMR_BLK_2M) {
		block_size = ARM64_MMU_L2_BLOCK_SIZE;
		final_level = 1;
	}
	else {
		block_size = ARM64_MMU_L3_PAGE_SIZE;
		final_level = 2;
	}

	while (len > 0) {
		ptp_t *cur_pgtbl = (ptp_t *)pgtbl;
		int level = 0;
		while (level <= final_level) {
			rc = get_next_ptp(cur_pgtbl, level, va, &next_ptp, &pte, true);
			(void)rc;  // suppress warning
			cur_pgtbl = next_ptp;
			level++;
		}

		if (flags & VMR_BLK_1G) {
			pte = &(next_ptp->ent[GET_L1_INDEX(va)]);
			set_pte_flags(pte, flags, KERNEL_PTE);
			pte->l1_block.is_valid = 1;
			pte->l1_block.is_table = 0;
			pte->l1_block.pfn = pa >> L1_INDEX_SHIFT;
		}
		else if (flags & VMR_BLK_2M) {
			pte = &(next_ptp->ent[GET_L2_INDEX(va)]);
			set_pte_flags(pte, flags, KERNEL_PTE);
			pte->l2_block.is_valid = 1;
			pte->l2_block.is_table = 0;
			pte->l2_block.pfn = pa >> L2_INDEX_SHIFT;
		}
		else {
			pte = &(next_ptp->ent[GET_L3_INDEX(va)]);
			set_pte_flags(pte, flags, KERNEL_PTE);
			pte->l3_page.is_valid = 1;
			pte->l3_page.is_page = 1;
			pte->l3_page.pfn = pa >> L3_INDEX_SHIFT;
		}

		len -= block_size;
		va += block_size;
		pa += block_size;
	}

	flush_tlb();
	return 0;
}


/*
 * unmap_range_in_pgtble: unmap the virtual address [va:va+len]
 * 
 * pgtbl @ ptr for the first level page table(pgd) virtual address
 * va @ start virtual address
 * len @ unmapping size
 * 
 * Hint: invoke get_next_ptp to get each level page table, don't 
 * forget the corner case that the virtual address is not mapped.
 * call flush_tlb() at the end of function
 * 
 */
int unmap_range_in_pgtbl(vaddr_t *pgtbl, vaddr_t va, size_t len)
{
	//lab2
	ptp_t *next_ptp;
	pte_t *pte;
	int rc;

	while (len > 0) {
		ptp_t *cur_pgtbl = (ptp_t *)pgtbl;
		u64 block_size;
		int level = 0;

		while (level <= 3) {
			// if the virtual address is not mapped, a new ptp should not be allocated
			rc = get_next_ptp(cur_pgtbl, level, va, &next_ptp, &pte, false);
			if (rc == -ENOMAPPING) {
				goto next;
			}
			if (rc == BLOCK_PTP || (rc == NORMAL_PTP && level == 3)) {
				if (level == 1) {
					pte->l1_block.is_valid = 0;
					// free_pages(pte->l1_block.pfn << L1_INDEX_SHIFT);  // no alloc so no free
				}
				else if (level == 2) {
					pte->l2_block.is_valid = 0;
					// free_pages(pte->l2_block.pfn << L2_INDEX_SHIFT);
				}
				else {
					pte->l3_page.is_valid = 0;
					// free_pages(pte->l3_page.pfn << L3_INDEX_SHIFT);
				}
				goto next;
			}
			cur_pgtbl = next_ptp;
			level++;
		}

next:
		block_size = 1 << (((3 - level) * PAGE_ORDER) + PAGE_SHIFT);
		len -= block_size;
		va += block_size;
	}

	flush_tlb();
	return 0;
}

