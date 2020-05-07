#include <common/asm.h>

#define __ASM__
#include <lib/registers.h>
#undef __ASM__

.extern init_c
.extern arm64_elX_to_el1

.extern vecotrs
.extern boot_cpu_stack
.extern secondary_boot_flag
.extern secondary_init_c
.extern MPIDR

BEGIN_FUNC(_start)
  /* get the processor id, and store it in register x8 */
	mrs	x8, mpidr_el1
	mov	x9, #0xc1000000
	bic	x8, x8, x9
	cbz	x8, primary

	/* Turn to el1 from other exception levels. */
	bl 	arm64_elX_to_el1

	/* Find the logical cpu id */
	ldr	x1, =MPIDR
	mov	x0, #0
find_id:
	ldr	x2, [x1, x0, lsl #3]
	add	x0, x0, #1
	cmp	x2, x8
	bne	find_id

	/* x8 stores the logical cpu id */
	sub	x8, x0, #1

	/* Prepare stack pointer and jump to C. */
	mov	x1, #0x1000
	mul	x1, x8, x1
	adr 	x0, boot_cpu_stack
	add	x0, x0, x1
	add	x0, x0, #0x1000
	mov	sp, x0

hang:
	/* CPU ID in x8 */
	mov	x1, #8
	mul	x2, x8, x1
	ldr	x1, =secondary_boot_flag
	add	x1, x1, x2
	ldr	x3, [x1]
	cbz	x3, hang

secondary_continue_boot:
	/* Set CPU id */
	mov	x0, x8
	bl 	secondary_init_c

proc_hang:
	bl	proc_hang


primary:
   	/* Turn to el1 from other exception levels. */
	bl 	arm64_elX_to_el1

	/* Prepare stack pointer and jump to C. */
	adr 	x0, boot_cpu_stack
	add 	x0, x0, #0x1000
	mov 	sp, x0

	bl 	init_c

	/* Should never be here */
	b 	proc_hang
END_FUNC(_start)
