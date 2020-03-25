#include "print/printf.h"
#include "boot.h"

#define PAGE_BITS           12
#define MASK(n)             (BIT(n) - 1)

#define ARM_1GB_BLOCK_BITS      30
#define ARM_2MB_BLOCK_BITS      21

#define PGDE_SIZE_BITS          3
#define PGD_BITS                9
#define PGD_SIZE_BITS           (PGD_BITS + PGDE_SIZE_BITS)

#define PUDE_SIZE_BITS          3
#define PUD_BITS                9
#define PUD_SIZE_BITS           (PUD_BITS + PUDE_SIZE_BITS)

#define PMDE_SIZE_BITS          3
#define PMD_BITS                9
#define PMD_SIZE_BITS           (PMD_BITS + PMDE_SIZE_BITS)

#define GET_PGD_INDEX(x)        \
	(((x) >> (ARM_2MB_BLOCK_BITS + PMD_BITS + PUD_BITS)) & MASK(PGD_BITS))
#define GET_PUD_INDEX(x)        \
	(((x) >> (ARM_2MB_BLOCK_BITS + PMD_BITS)) & MASK(PUD_BITS))
#define GET_PMD_INDEX(x)        \
	(((x) >> (ARM_2MB_BLOCK_BITS)) & MASK(PMD_BITS))

typedef unsigned long paddr_t;
typedef unsigned long vaddr_t;

typedef unsigned long uint64_t;
typedef unsigned long uintptr_t;

/* Paging structures for kernel mapping */
uint64_t _boot_pgd_up[BIT(PGD_BITS)] ALIGN(BIT(PGD_SIZE_BITS));
uint64_t _boot_pud_up[BIT(PUD_BITS)] ALIGN(BIT(PUD_SIZE_BITS));
uint64_t _boot_pmd_up[BIT(PMD_BITS)] ALIGN(BIT(PMD_SIZE_BITS));

/* Paging structures for identity mapping */
uint64_t _boot_pgd_down[BIT(PGD_BITS)] ALIGN(BIT(PGD_SIZE_BITS));
uint64_t _boot_pud_down[BIT(PUD_BITS)] ALIGN(BIT(PUD_SIZE_BITS));
uint64_t _boot_pmd_down[BIT(PMD_BITS)] ALIGN(BIT(PMD_SIZE_BITS));

/*
 * Create a "boot" page table, which contains a 1:1 mapping below
 * the kernel's first vaddr, and a virtual-to-physical mapping above the
 * kernel's first vaddr.
 */
void init_boot_pt(void)
{
	uint64_t i;

	vaddr_t first_vaddr = KERNEL_VADDR;
	paddr_t first_paddr = 0;

	printf("[BOOT] init boot page table: first_vaddr=0x%lx first_paddr=0x%lx\r\n",
	       first_vaddr, first_paddr);

	/* Map virtual address below 1G to physical address. */
	_boot_pgd_down[0] = ((uintptr_t) _boot_pud_down) | BIT(1) | BIT(0);

	//ttbr0
	//0~2G
	for (i = 0; i < 2; ++i) {
		_boot_pud_down[i] = (i << ARM_1GB_BLOCK_BITS)
		    | BIT(54)   /* UXN */
		    | BIT(10)	/* access flag */
		    | (3 << 8)  /* shareability */
		    | (4 << 2)	/* MT_NORMAL memory */
		    | BIT(0);	/* 1G block */
	}

	//2G~4G
	for (i = 2; i < 4; ++i) {
		_boot_pud_down[i] = (i << ARM_1GB_BLOCK_BITS)
		    | BIT(54)   /* UXN */
		    | BIT(10)	/* access flag */
		    | (0 << 2)	/* strongly ordered device memory */
		    | BIT(0);	/* 1G block */
	}


	_boot_pgd_up[GET_PGD_INDEX(first_vaddr)]
	    = ((uintptr_t) _boot_pud_up) | BIT(1) | BIT(0);

	_boot_pud_up[GET_PUD_INDEX(first_vaddr)]
	    = ((uintptr_t) _boot_pmd_up) | BIT(1) | BIT(0);
	printf("index %lx\n",GET_PUD_INDEX(first_vaddr));
	printf("_boot_pgd_up is %lx\n",_boot_pgd_up);
	//0~128m
	for (i = 0; i < 64; ++i) {
		_boot_pmd_up[i] = ((i << ARM_2MB_BLOCK_BITS) + first_paddr)
			| BIT(54)
		    | BIT(10)	/* access flag */
		    | (3 << 8)  /* shareability */
		    | (4 << 2)	/* MT_NORMAL memory */
		    | BIT(0);	/* 1G block */
	}

	//128~1G
	for (i = 128; i < 512; ++i) {
		_boot_pmd_up[i] = ((i << ARM_2MB_BLOCK_BITS) + first_paddr)
		    | BIT(54)   /* UXN */
		    | BIT(10)	/* access flag */
		    | (0 << 2)	/* strongly ordered device memory */
		    | BIT(0);	/* 1G block */
	}
	for (i = 1; i < 4; ++i) {
		_boot_pud_up[i] = (i << ARM_1GB_BLOCK_BITS)
		    | BIT(54)   /* UXN */
		    | BIT(10)	/* access flag */
		    | (0 << 2)
		    | BIT(0);	/* 1G block */
	}
}
