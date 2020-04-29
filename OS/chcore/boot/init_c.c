#include "print/printf.h"
#include "print/uart.h"
#include "boot.h"
#include "image.h"

ALIGN(16) VISIBLE
char boot_cpu_stack[CONFIG_MAX_NUM_CPUS][BIT(PAGE_BITS)] = { 0 };
long secondary_boot_flag[CONFIG_MAX_NUM_CPUS + 1] = { 0 };

void init_c(void)
{
	/* Initialize UART before enabling MMU. */
	early_uart_init();

	/* Initialize Boot Page Table. */
	printf("[BOOT] Install boot page table\r\n");
	init_boot_pt();

	/* Enable MMU. */
	printf("[BOOT] Enable el1 MMU\r\n");
	el1_mmu_activate();

	/* Call Kernel Main. */
	printf("[BOOT] Jump to kernel main at 0x%lx\r\n\n\n\n\n", start_kernel);
	start_kernel(secondary_boot_flag);
	
	printf("[BOOT] Should not be here!\r\n");
}

void secondary_init_c(int cpuid)
{
	el1_mmu_activate();
	secondary_cpu_boot(cpuid);
}