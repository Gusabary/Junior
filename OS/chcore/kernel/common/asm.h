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

#define __ASM__

#define BEGIN_FUNC(_name)	\
	.global _name;		\
    .type   _name, %function;	\
_name:

#define END_FUNC(_name)		\
	.size _name, .-_name

#define	EXPORT(symbol)		\
	.globl	symbol;		\
	symbol:

#define BEGIN_FUNC_STATIC(_name)  \
	.type   _name, %function; \
_name:

#define END_FUNC_STATIC(_name) \
	.size _name, .-_name

#define LOCAL_DATA(x) .type x,1; x:
#define DATA(x) .global x; .hidden x; LOCAL_DATA(x)
#define END_DATA(x) .size x, .-x

/*
 * LEAF - declare leaf routine
 */
#define LEAF(symbol)                            \
	.globl  symbol;                         \
	.align  2;                              \
	.type   symbol,@function;               \
	.ent    symbol,0;                       \
	symbol:         .frame  sp,0,ra

/*
 * NESTED - declare nested routine entry point
 */
#define NESTED(symbol, framesize, rpc)		\
	.globl  symbol;                         \
	.align  2;                              \
	.type   symbol,@function;               \
	.ent    symbol,0;                       \
	symbol:         .frame  sp, framesize, rpc


/*
 * END - mark end of function
 */
#define END(function)				\
	.end    function;                       \
	.size   function,.-function

#define	EXPORT(symbol)				\
	.globl	symbol;				\
	symbol:

#define FEXPORT(symbol)				\
	.globl	symbol; 			\
	.type	symbol,@function;		\
	symbol:

