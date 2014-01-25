#ifndef EXCEPTION_H
#define EXCEPTION_H

//Exception settings
#define EXCEPTION_COUNT				10
#define EXCEPTION_TABLE				((void*) 0xFFFF0000)

//Exceptions constants
#define EXCEPTION_RESET				0
#define EXCEPTION_UNDEFINED			0x1
#define EXCEPTION_SWI				0x2
#define EXCEPTION_PREFETCH_ABORT	0x3
#define EXCEPTION_DATA_ABORT		0x4
#define EXCEPTION_IRQ				0x6
#define EXCEPTION_FIQ				0x7

//Prototypes
void exception_init();
void __attribute__((interrupt("naked"))) exception_data_abort();
void __attribute__((interrupt("naked"))) exception_irq();
void __attribute__((interrupt("naked"))) exception_swi();

#endif
