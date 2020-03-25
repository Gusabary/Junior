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

#include <common/uart.h>
#include <common/types.h>

#define MAX_INT_BUFF_SIZE  64

typedef __builtin_va_list va_list;
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

static void printk_write_string(const char *str)
{
	int i;

	for (i = 0; str[i] != 0; i++)
		uart_send(str[i]);
}

// this function print number `n` in the base of `base` (base > 1)
// you may need to call `printk_write_string`
// you do not need to print prefix like "0x", "0"...
// Remember the most significant digit is printed first.
static void printk_write_num(int base, unsigned long long n, int neg)
{
  	static const char hex[] = "0123456789abcdef";
	char buff[MAX_INT_BUFF_SIZE];

  	// fill the buf
	int i = 0;
	for (; n > 0; i++) {
		buff[i] = hex[n % base];
		n /= base;
	}
	if (i == 0) {  // n == 0
		buff[i++] = '0';
	}
	if (neg) {  // if negative, base must be 10
		buff[i++] = '-';
	}
	buff[i] = '\0';

	// reverse the buf
	for (char *head = buff, *tail = buff + i - 1; head < tail; head++, tail--) {
		char tmp = *head;
		*head = *tail;
		*tail = tmp;
	}
	
	printk_write_string(buff);
}

void printk_format(char *format, va_list args)
{
	int d, i, base;
	char c, *s;
	unsigned long p;
	long l;
	int escape_mode = 0;

	/* Iterate over the format list. */
	for (i = 0; format[i] != 0; i++) {
		/* Handle simple characters. */
		if (!escape_mode && format[i] != '%') {
			if ( format[i] == '\n' ){
				uart_send('\n');
				uart_send('\r');
			} else {
				uart_send(format[i]);
			}
			continue;
		}

		/* Handle the percent escape character. */
		if (format[i] == '%') {
			if (!escape_mode) {
				/* Entering escape mode. */
				escape_mode = 1;
			} else {
				/* Already in escape mode; print a percent. */
				uart_send(format[i]);
				escape_mode = 0;
			}
			continue;
		}

		/* Handle the modifier. */
		switch (format[i]) {
			/* Ignore printf modifiers we don't support. */
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
		case '.':
			break;

			/* String. */
		case 's':
			s = va_arg(args, char *);
			printk_write_string(s);
			escape_mode = 0;
			break;

			/* Pointers. */
		case 'p':
			p = va_arg(args, unsigned long);
			printk_write_num(16, p, 0);
			escape_mode = 0;
			break;

			/* Hex number. */
		case 'x':
			d = va_arg(args, int);
			printk_write_num(16, d, 0);
			escape_mode = 0;
			break;

			/* Decimal number. */
		case 'd':
			d = va_arg(args, int);
			if (d >= 0)
				printk_write_num(10, d, 0);
			else /* negtive number */
				printk_write_num(10, -d, 1);
			escape_mode = 0;
			break;
		case 'u':
			d = va_arg(args, int);
			printk_write_num(10, d, 0);
			escape_mode = 0;
			break;

			/* Hex number. */
		case 'b':
			d = va_arg(args, int);
			printk_write_num(2, d, 0);
			escape_mode = 0;
			break;

			/* Character. */
		case 'c':
			c = va_arg(args, int);
			uart_send(c);
			escape_mode = 0;
			break;

			/* Long number. */
		case 'l': {
			int neg = 0;

			l = va_arg(args, long);

			switch (format[++i]) {
			case 'd':
				base = 10;
				if (l < 0) {
					neg = 1;
					l = -l;
				}
				break;
			case 'u':
				base = 10;
				break;

			case 'x':
				base = 16;
				break;

			case 'b':
				base = 2;
				break;

			default:
				uart_send('?');
				continue;
			}

			printk_write_num(base, l, neg);
			escape_mode = 0;
			break;
		}

			/* Unknown. */
		default:
			uart_send('?');
			escape_mode = 0;
			break;
		}
	}
}

void printk(char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printk_format(fmt, va);
	va_end(va);
}

void break_point()
{
	printk("[ChCore] Lab stalling ... \n");
}
