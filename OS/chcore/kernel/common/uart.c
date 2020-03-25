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

#include <common/machine.h>
#include <common/types.h>
#include "tools.h"
#include <common/uart.h>

u32 uart_lsr(void)
{
	return get32(AUX_MU_LSR_REG);
}

u32 uart_recv(void)
{
	while (1) {
		if (uart_lsr() & 0x01)
			break;
	}

	return (get32(AUX_MU_IO_REG) & 0xFF);
}

u32 nb_uart_recv(void)
{
	if(uart_lsr() & 0x01)
		return (get32(AUX_MU_IO_REG) & 0xFF);
	else
		return NB_UART_NRET;
}

void uart_send(u32 c)
{
	while (1) {
		if (uart_lsr() & 0x20)
			break;
	}
	put32(AUX_MU_IO_REG, c);
}

void uart_init(void)
{
	u32 ra;

	put32(AUX_ENABLES, 1);
	put32(AUX_MU_IER_REG, 0);
	put32(AUX_MU_CNTL_REG, 0);
	put32(AUX_MU_LCR_REG, 3);
	put32(AUX_MU_MCR_REG, 0);
	put32(AUX_MU_IER_REG, 0);
	put32(AUX_MU_IIR_REG, 0xC6);
	put32(AUX_MU_BAUD_REG, 270);
	ra = get32(BCM2835_GPIO_GPFSEL1);
	ra &= ~(7 << 12);	//gpio14
	ra |= 2 << 12;		//alt5
	ra &= ~(7 << 15);	//gpio15
	ra |= 2 << 15;		//alt5
	put32(BCM2835_GPIO_GPFSEL1, ra);
	put32(BCM2835_GPIO_GPPUD, 0);
	//TODO dealy _sleep(0);
	put32(BCM2835_GPIO_GPPUDCLK0, (1 << 14) | (1 << 15));
	//_sleep(0);
	put32(BCM2835_GPIO_GPPUDCLK0, 0);
	put32(AUX_MU_CNTL_REG, 3);

	/* Flush the screen */
	uart_send(12);
	uart_send(27);
	uart_send('[');
	uart_send('2');
	uart_send('J');
}
