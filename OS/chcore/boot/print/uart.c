#include "uart.h"

void early_uart_init(void)
{
	unsigned int ra;

	early_put32(AUX_ENABLES, 1);
	early_put32(AUX_MU_IER_REG, 0);
	early_put32(AUX_MU_CNTL_REG, 0);
	early_put32(AUX_MU_LCR_REG, 3);
	early_put32(AUX_MU_MCR_REG, 0);
	early_put32(AUX_MU_IER_REG, 0);
	early_put32(AUX_MU_IIR_REG, 0xC6);
	early_put32(AUX_MU_BAUD_REG, 270);
	ra = early_get32(GPFSEL1);
	ra &= ~(7 << 12);	//gpio14
	ra |= 2 << 12;		//alt5
	ra &= ~(7 << 15);	//gpio15
	ra |= 2 << 15;		//alt5
	early_put32(GPFSEL1, ra);
	early_put32(GPPUD, 0);
	delay(150);
	early_put32(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
	early_put32(GPPUDCLK0, 0);
	early_put32(AUX_MU_CNTL_REG, 3);
}

unsigned int early_uart_lsr(void)
{
	return early_get32(AUX_MU_LSR_REG);
}

unsigned int early_uart_recv(void)
{
	while (1) {
		if (early_uart_lsr() & 0x01)
			break;
	}
	return early_get32(AUX_MU_IO_REG) & 0xFF;
}

void early_uart_send(unsigned int c)
{
	while (1) {
		if (early_uart_lsr() & 0x20)
			break;
	}
	early_put32(AUX_MU_IO_REG, c);
}

void uart_send_string(char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
		early_uart_send((char)str[i]);
}

void uart_putc(char c)
{
	early_uart_send(c);
}
