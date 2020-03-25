#pragma once

#define PHYSADDR_OFFSET     0x3F200000uL
#define GPFSEL1            (PHYSADDR_OFFSET + 0x00000004)
#define GPSET0             (PHYSADDR_OFFSET + 0x0000001C)
#define GPCLR0             (PHYSADDR_OFFSET + 0x00000028)
#define GPPUD              (PHYSADDR_OFFSET + 0x00000094)
#define GPPUDCLK0          (PHYSADDR_OFFSET + 0x00000098)

#define AUX_ENABLES        (PHYSADDR_OFFSET + 0x00015004)
#define AUX_MU_IO_REG      (PHYSADDR_OFFSET + 0x00015040)
#define AUX_MU_IER_REG     (PHYSADDR_OFFSET + 0x00015044)
#define AUX_MU_IIR_REG     (PHYSADDR_OFFSET + 0x00015048)
#define AUX_MU_LCR_REG     (PHYSADDR_OFFSET + 0x0001504C)
#define AUX_MU_MCR_REG     (PHYSADDR_OFFSET + 0x00015050)
#define AUX_MU_LSR_REG     (PHYSADDR_OFFSET + 0x00015054)
#define AUX_MU_MSR_REG     (PHYSADDR_OFFSET + 0x00015058)
#define AUX_MU_SCRATCH     (PHYSADDR_OFFSET + 0x0001505C)
#define AUX_MU_CNTL_REG    (PHYSADDR_OFFSET + 0x00015060)
#define AUX_MU_STAT_REG    (PHYSADDR_OFFSET + 0x00015064)
#define AUX_MU_BAUD_REG    (PHYSADDR_OFFSET + 0x00015068)

extern void early_put32(unsigned long int addr, unsigned int ch);
extern unsigned int early_get32(unsigned long int addr);
extern void delay(unsigned long time);

void early_uart_init(void);
void uart_putc(char ch);
