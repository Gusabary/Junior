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

#include <common/types.h>

/* x most significant bits should be all 0 or 1. */
/* aka translation input size */
#define TCR_T0SZ(x)  ((64-(x)) << 0)
#define TCR_T1SZ(x)  ((64-(x)) << 16)
#define TCR_TxSZ(x)  (TCR_T0SZ(x) | TRC_T1SZ(x))

/* 00=4KB, 01=16KB, 11=64KB */
/* Translation granularity, or the page size */

#define TCR_TG0_4K   (0x0 << 14)
#define TCR_TG0_16K  (0x1 << 14)
#define TCR_TG0_64K  (0x3 << 14)

#define TCR_TG1_4K   (0x0 << 30)
#define TCR_TG1_16K  (0x1 << 30)
#define TCR_TG1_64K  (0x3 << 30)

/* TCR.{I}PS PA SIZE, aka translation output size */
/* 000=32bits 4GB */
/* 001=36bits 64GB */
/* 010=40bits 1TB */
/* 011=42bits 4TB */
/* 100=44bits 16TB */
/* 101=48bits 256TB */
/* 110=52bits 4PB */
#define TCR_PS_4G    ((0x0llu) << 32)
#define TCR_PS_64G   ((0x1llu) << 32)
#define TCR_PS_1T    ((0x2llu) << 32)
#define TCR_PS_4T    ((0x3llu) << 32)
#define TCR_PS_16T   ((0x4llu) << 32)
#define TCR_PS_256T  ((0x5llu) << 32)
#define TCR_PS_4P    ((0x6llu) << 32)


/**
 * TTBR1's shareability:
 *  NS: non-shareable
 *  OS: outer sharable
 *  IS: inner sharable
 */
#define TCR_SH1_NS  (0x0 << 28)
#define TCR_SH1_OS  (0x2 << 28)
#define TCR_SH1_IS  (0x3 << 28)
/* TTBR0's shareability */
#define TCR_SH0_NS  (0x0 << 12)
#define TCR_SH0_OS  (0x2 << 12)
#define TCR_SH0_IS  (0x3 << 12)

/**
 * Outer cacheability for TBBR1:
 *  NC: non-cacheable
 *  WBWA: write-back read-allocate write-allocate cacheable
 *  WTnWA: write-back read-allocate no write-allocate cacheable
 *  WBnWA: write-back read-allocate no write-allocate cacheable
 */
#define TCR_ORGN1_NC     (0x0 << 26)
#define TCR_ORGN1_WBWA   (0x1 << 26)
#define TCR_ORGN1_WTnWA  (0x2 << 26)
#define TCR_ORGN1_WBnWA  (0x3 << 26)
/* Inner shareability for TBBR1 */
#define TCR_IRGN1_NC     (0x0 << 24)
#define TCR_IRGN1_WBWA   (0x1 << 24)
#define TCR_IRGN1_WTnWA  (0x2 << 24)
#define TCR_IRGN1_WBnWA  (0x3 << 24)
/* Outer shareability for TBBR0 */
#define TCR_ORGN0_NC     (0x0 << 10)
#define TCR_ORGN0_WBWA   (0x1 << 10)
#define TCR_ORGN0_WTnWA  (0x2 << 10)
#define TCR_ORGN0_WBnWA  (0x3 << 10)
/* Inner shareability for TBBR0 */
#define TCR_IRGN0_NC     (0x0 << 8)
#define TCR_IRGN0_WBWA   (0x1 << 8)
#define TCR_IRGN0_WTnWA  (0x2 << 8)
#define TCR_IRGN0_WBnWA  (0x3 << 8)

#define INNER_SHAREABLE  (0x3)
#define NORMAL_MEMORY    (0x4)

/**
 * Whether a translation table walk is performed on a TLB miss, for an address
 * that is translated using TTBR1_EL1/TTBR0_EL1.
 */
#define TCR_EPD1_WALK   (0x0 << 23)
#define TCR_EPD1_FAULT  (0x1 << 23)
#define TCR_EPD0_WALK   (0x0 << 7)
#define TCR_EPD0_FAULT  (0x1 << 7)

/* Who defines the ASID */
#define TCR_A1_TTBR0  (0x0 << 22)
#define TCR_A1_TTBR1  (0x1 << 22)

/* TCR_EL1 */

/**
 * Four-level page table for 4KB pages
 *  - p0d_t is the address of the 4K page
 *  - each p1d_t contains 512 p1e_t that points to one p0d_t
 *  - each p2d_t contains 512 p2e_t that points to one p1d_t
 *  - each p3d_t contains 512 p3e_t that points to one p2d_t
 *  - each p4d_t contains 512 p4e_t that points to one p3d_t
 *
 * Relations to the ARM document terminalogies:
 *  p1d_t: level 3 table
 *  P2d_t: level 2 table
 *  p3d_t: level 1 table
 *  p4d_t: level 0 table
 *
 */

/* Table attributes */
#define ARM64_MMU_ATTR_TBL_AP_TABLE_NOEFFECT      (0)
#define ARM64_MMU_ATTR_TBL_AP_TABLE_NOEL0         (1)
#define ARM64_MMU_ATTR_TBL_AP_TABLE_NOWRITE       (2)
#define ARM64_MMU_ATTR_TBL_AP_TABLE_NOACCESS      (3)

/* Block/Page attributes */
#define ARM64_MMU_ATTR_PAGE_AP_HIGH_RW_EL0_NA     (0)
#define ARM64_MMU_ATTR_PAGE_AP_HIGH_RW_EL0_RW     (1)
#define ARM64_MMU_ATTR_PAGE_AP_HIGH_RO_EL0_NA     (2)
#define ARM64_MMU_ATTR_PAGE_AP_HIGH_RO_EL0_RO     (3)
#define ARM64_MMU_ATTR_PAGE_UX                    (0)
#define ARM64_MMU_ATTR_PAGE_UXN                   (1)
#define ARM64_MMU_ATTR_PAGE_PX                    (0)
#define ARM64_MMU_ATTR_PAGE_PXN                   (1)

#define ARM64_MMU_ATTR_PAGE_AF_NONE               (0)
#define ARM64_MMU_ATTR_PAGE_AF_ACCESSED           (1)

#define ARM64_MMU_PTE_INVALID_MASK                (1 << 0)
#define ARM64_MMU_PTE_TABLE_MASK                  (1 << 1)

#define IS_PTE_INVALID(pte) (!((pte) & ARM64_MMU_PTE_INVALID_MASK))
#define IS_PTE_TABLE(pte) (!!((pte) & ARM64_MMU_PTE_TABLE_MASK))


#define PAGE_SHIFT                                (12)
#ifndef PAGE_SIZE
#define PAGE_SIZE                                 (1 << (PAGE_SHIFT))
#endif
#define PAGE_MASK                                 (PAGE_SIZE - 1)
#define PAGE_ORDER                                (9)

#define PTP_ENTRIES				  (1 << PAGE_ORDER)
#define L3					  (3)
#define L2					  (2)
#define L1					  (1)
#define L0					  (0)

#define PTP_INDEX_MASK				  ((1 << (PAGE_ORDER)) - 1)
#define L0_INDEX_SHIFT			    ((3 * PAGE_ORDER) + PAGE_SHIFT)
#define L1_INDEX_SHIFT			    ((2 * PAGE_ORDER) + PAGE_SHIFT)
#define L2_INDEX_SHIFT			    ((1 * PAGE_ORDER) + PAGE_SHIFT)
#define L3_INDEX_SHIFT			    ((0 * PAGE_ORDER) + PAGE_SHIFT)

#define GET_L0_INDEX(addr) ((addr >> L0_INDEX_SHIFT) & PTP_INDEX_MASK)
#define GET_L1_INDEX(addr) ((addr >> L1_INDEX_SHIFT) & PTP_INDEX_MASK)
#define GET_L2_INDEX(addr) ((addr >> L2_INDEX_SHIFT) & PTP_INDEX_MASK)
#define GET_L3_INDEX(addr) ((addr >> L3_INDEX_SHIFT) & PTP_INDEX_MASK)

#define PGTBL_4K_BITS                             (9)
#define PGTBL_4K_ENTRIES                          (1 << (PGTBL_4K_BITS))
#define PGTBL_4K_MAX_INDEX                        ((PGTBL_4K_ENTRIES) - 1)

#define ARM64_MMU_L1_BLOCK_ORDER                  (18)
#define ARM64_MMU_L2_BLOCK_ORDER                  (9)
#define ARM64_MMU_L3_PAGE_ORDER                   (0)

#define ARM64_MMU_L0_BLOCK_PAGES  (PTP_ENTRIES * ARM64_MMU_L1_BLOCK_PAGES)
#define ARM64_MMU_L1_BLOCK_PAGES  (1UL << ARM64_MMU_L1_BLOCK_ORDER)
#define ARM64_MMU_L2_BLOCK_PAGES  (1UL << ARM64_MMU_L2_BLOCK_ORDER)
#define ARM64_MMU_L3_PAGE_PAGES   (1UL << ARM64_MMU_L3_PAGE_ORDER)

#define L0_PER_ENTRY_PAGES	  (ARM64_MMU_L0_BLOCK_PAGES)
#define L1_PER_ENTRY_PAGES	  (ARM64_MMU_L1_BLOCK_PAGES)
#define L2_PER_ENTRY_PAGES        (ARM64_MMU_L2_BLOCK_PAGES)
#define L3_PER_ENTRY_PAGES	  (ARM64_MMU_L3_PAGE_PAGES)

#define ARM64_MMU_L1_BLOCK_SIZE   (ARM64_MMU_L1_BLOCK_PAGES << PAGE_SHIFT)
#define ARM64_MMU_L2_BLOCK_SIZE   (ARM64_MMU_L2_BLOCK_PAGES << PAGE_SHIFT)
#define ARM64_MMU_L3_PAGE_SIZE    (ARM64_MMU_L3_PAGE_PAGES << PAGE_SHIFT)

#define ARM64_MMU_L1_BLOCK_MASK   (ARM64_MMU_L1_BLOCK_SIZE - 1)
#define ARM64_MMU_L2_BLOCK_MASK   (ARM64_MMU_L2_BLOCK_SIZE - 1)
#define ARM64_MMU_L3_PAGE_MASK    (ARM64_MMU_L3_PAGE_SIZE - 1)

#define GET_VA_OFFSET_L1(va)      (va & ARM64_MMU_L1_BLOCK_MASK)
#define GET_VA_OFFSET_L2(va)      (va & ARM64_MMU_L2_BLOCK_MASK)
#define GET_VA_OFFSET_L3(va)      (va & ARM64_MMU_L3_PAGE_MASK)

#define PTE_DESCRIPTOR_INVALID                    (0)
#define PTE_DESCRIPTOR_BLOCK                      (1)
#define PTE_DESCRIPTOR_TABLE                      (3)
#define PTE_DESCRIPTOR_MASK                       (3)

/* table format */
typedef union {
	struct {
		u64 is_valid        : 1,
		    is_table        : 1,
		    ignored1        : 10,
		    next_table_addr : 36,
		    reserved        : 4,
		    ignored2        : 7,
		    PXNTable        : 1,   // Privileged Execute-never for next level
		    XNTable         : 1,   // Execute-never for next level
		    APTable         : 2,   // Access permissions for next level
		    NSTable         : 1;
	} table;
	struct {
		u64 is_valid        : 1,
		    is_table        : 1,
		    attr_index      : 3,   // Memory attributes index
		    NS              : 1,   // Non-secure
		    AP              : 2,   // Data access permissions
		    SH              : 2,   // Shareability
		    AF              : 1,   // Accesss flag
		    nG              : 1,   // Not global bit
		    reserved1       : 4,
		    nT              : 1,
		    reserved2       : 13,
		    pfn             : 18,
		    reserved3       : 2,
		    GP              : 1,
		    reserved4       : 1,
		    DBM             : 1,   // Dirty bit modifier
		    Contiguous      : 1,
		    PXN             : 1,   // Privileged execute-never
		    UXN             : 1,   // Execute never
		    soft_reserved   : 4,
		    PBHA            : 4;   // Page based hardware attributes
	} l1_block;
	struct {
		u64 is_valid        : 1,
		    is_table        : 1,
		    attr_index      : 3,   // Memory attributes index
		    NS              : 1,   // Non-secure
		    AP              : 2,   // Data access permissions
		    SH              : 2,   // Shareability
		    AF              : 1,   // Accesss flag
		    nG              : 1,   // Not global bit
		    reserved1       : 4,
		    nT              : 1,
		    reserved2       : 4,
		    pfn             : 27,
		    reserved3       : 2,
		    GP              : 1,
		    reserved4       : 1,
		    DBM             : 1,   // Dirty bit modifier
		    Contiguous      : 1,
		    PXN             : 1,   // Privileged execute-never
		    UXN             : 1,   // Execute never
		    soft_reserved   : 4,
		    PBHA            : 4;   // Page based hardware attributes
	} l2_block;
	struct {
		u64 is_valid        : 1,
		    is_page         : 1,
		    attr_index      : 3,   // Memory attributes index
		    NS              : 1,   // Non-secure
		    AP              : 2,   // Data access permissions
		    SH              : 2,   // Shareability
		    AF              : 1,   // Accesss flag
		    nG              : 1,   // Not global bit
		    pfn             : 36,
		    reserved        : 3,
		    DBM             : 1,   // Dirty bit modifier
		    Contiguous      : 1,
		    PXN             : 1,   // Privileged execute-never
		    UXN             : 1,   // Execute never
		    soft_reserved   : 4,
		    PBHA            : 4,   // Page based hardware attributes
		    ignored         : 1;
	} l3_page;
	u64 pte;
} pte_t;

/* page_table_page type */
typedef struct {
	pte_t ent[1 << PGTBL_4K_BITS];
} ptp_t;

//typedef u64 vmr_prop_t;
//#define VMR_READ  (1 << 0)
//#define VMR_WRITE (1 << 1)
//#define VMR_EXEC  (1 << 2)

/* functions */
//int map_range_in_pgtbl(ptp_t *pgtbl, vaddr_t va, paddr_t pa,
//		       size_t len, vmr_prop_t flags);
//int unmap_range_in_pgtbl(ptp_t *pgtbl, vaddr_t va, size_t len);
