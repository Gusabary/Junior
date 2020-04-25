#include <lib/print.h>
#include <lib/syscall.h>
#include <lib/type.h>

u64 syscall(u64 sys_no, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4,
	    u64 arg5, u64 arg6, u64 arg7, u64 arg8)
{

	u64 ret = 0;
	/*
	 * Lab3: Your code here
	 * Use inline assembly to store arguments into x0 to x7, store syscall number to x8,
	 * And finally use svc to execute the system call. After syscall returned, don't forget
	 * to move return value from x0 to the ret variable of this function
	 */
	// I don't know why 'mov x8, %1' must be the first instruction 
	asm volatile("mov x8, %1\n"
				 "mov x0, %2\n"
				 "mov x1, %3\n"
				 "mov x2, %4\n"
				 "mov x3, %5\n"
				 "mov x4, %6\n"
				 "mov x5, %7\n"
				 "mov x6, %8\n"
				 "mov x7, %9\n"
				 "svc 0\n"
				 "mov %0, x0\n"
				 : "=r"(ret)
				 : "r"(sys_no), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3),
				   "r"(arg4), "r"(arg5), "r"(arg6), "r"(arg7));

	return ret;
}

static u64 syscall_helper1(u64 sys_no, u64 arg0) {
	return syscall(sys_no, arg0, 0, 0, 0, 0, 0, 0, 0, 0);
}

static u64 syscall_helper2(u64 sys_no, u64 arg0, u64 arg1) {
	return syscall(sys_no, arg0, arg1, 0, 0, 0, 0, 0, 0, 0);
}

// static u64 syscall_helper3(u64 sys_no, u64 arg0, u64 arg1, u64 arg2) {
// 	return syscall(sys_no, arg0, arg1, arg2, 0, 0, 0, 0, 0, 0);
// }

static u64 syscall_helper4(u64 sys_no, u64 arg0, u64 arg1, u64 arg2, u64 arg3) {
	return syscall(sys_no, arg0, arg1, arg2, arg3, 0, 0, 0, 0, 0);
}

/*
 * Lab3: your code here:
 * Finish the following system calls using helper function syscall
 */
void usys_putc(char ch)
{
	syscall_helper1(SYS_putc, ch);
}

void usys_exit(int ret)
{
	syscall_helper1(SYS_exit, ret);
}

int usys_create_pmo(u64 size, u64 type)
{
	return syscall_helper2(SYS_create_pmo, size, type);
}

int usys_map_pmo(u64 process_cap, u64 pmo_cap, u64 addr, u64 rights)
{
	return syscall_helper4(SYS_map_pmo, process_cap, pmo_cap, addr, rights);
}

u64 usys_handle_brk(u64 addr)
{
	return syscall_helper1(SYS_handle_brk, addr);
}

/* Here finishes all syscalls need by lab3 */
