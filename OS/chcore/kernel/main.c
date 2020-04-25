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

#include <common/kprint.h>
#include <lib/machine.h>
#include <common/macro.h>
#include <common/mm.h>
#include <common/uart.h>
#include <common/vars.h>
#include <exception/exception.h>
#include <lib/types.h>
#include <process/thread.h>
#include <sched/sched.h>

ALIGN(STACK_ALIGNMENT)
char kernel_stack[PLAT_CPU_NUM][KERNEL_STACK_SIZE];

int mon_backtrace();

// Test the stack backtrace function (lab 1 only)
void
test_backtrace(long x)
{
	kinfo("entering test_backtrace %d\n", x);
	if (x > 0)
		test_backtrace(x-1);
	else
		mon_backtrace(0, 0, 0);
	kinfo("leaving test_backtrace %d\n", x);
}

void main(void *addr)
{
	/* Init uart */
	uart_init();
	kinfo("[ChCore] uart init finished\n");

  	kinfo("6828 decimal is %x hex!\n", 6828);

  	test_backtrace(5);

	mm_init(NULL);
	kinfo("mm init finished\n");
	
	/* Init exception vector */
	exception_init();
	kinfo("[ChCore] interrupt init finished\n");


#ifdef TEST
	/* Create initial thread here*/
	process_create_root(TEST);
	kinfo("[ChCore] root thread init finished\n");
#else
	/* We will run the kernel test if you do not type make bin=xxx */
	break_point();
	BUG("No given TEST!");
#endif 

	eret_to_thread(switch_context());

	/* Should provide panic and use here */
	BUG("[FATAL] Should never be here!\n");
}

