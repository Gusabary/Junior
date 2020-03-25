#include <common/asm.h>

#define __ASM__
#include "../lib/registers.h"
#undef __ASM__

.extern init_c
.extern arm64_elX_to_el1

.extern boot_cpu_stack

BEGIN_FUNC(_start)
  /* get the processor id, and store it in register x8 */
	mrs	x8, mpidr_el1
	mov	x9, #0xc1000000
	bic	x8, x8, x9
	cbz	x8, primary

  /* hang all secondary processors before we intorduce multi-processors */
secondary_hang:
	bl secondary_hang

proc_hang:
	bl proc_hang
	
primary:
  /* Turn to el1 from other exception levels. */
	bl 	arm64_elX_to_el1

	/* Prepare stack pointer and jump to C. */
	adr 	x0, boot_cpu_stack
	add 	x0, x0, #0x1000
	mov 	sp, x0

	/* Set up boot exceptions vector */
	#adr 	x0, boot_vector
	#msr 	vbar_el1, x0

	bl 	init_c

	/* Should never be here */
	b 	proc_hang
END_FUNC(_start)
