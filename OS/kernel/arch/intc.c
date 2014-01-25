#include <stdint.h>
#include <arch/intc.h>

//Basic IRQs
#define	INTC_IRQ_TIMER				0
#define	INTC_IRQ_mailbox 			1
#define	INTC_IRQ_doorbell0			2
#define	INTC_IRQ_doorbell1			3
#define	INTC_IRQ_gpu0_halted		4
#define	INTC_IRQ_gpu1_halted		5
#define	INTC_IRQ_illegal1			6
#define	INTC_IRQ_illegal0			7
#define INTC_IRQ_COUNT				8

//Controller IO
#define INTC_BASE					0xF200B000
#define INTC_PENDING_BASIC			((volatile uint32_t *)(INTC_BASE+0x200))
#define INTC_ENABLE_BASIC			((volatile uint32_t *)(INTC_BASE+0x218))

//Array of IRQ handler pointers
void (*intc_handlers[INTC_IRQ_COUNT][2])();

//Prototypes
int_fast8_t _intc_get_pending();

/*
 * intc_handle
 *
 * Call IRQ's handler
 */
void __attribute__((used)) intc_handle() {
	uint8_t irqn = _intc_get_pending();
	intc_handlers[irqn][0](); //Clear the interrupt	
	intc_handlers[irqn][1](); //Call the interrupt handler
}

/**
 * intc_get_pending()
 *
 * Get the pending IRQ number
 */
int_fast8_t _intc_get_pending() {
	uint_fast16_t shifts = 0;
	uint32_t mask = *INTC_PENDING_BASIC;
	while (1) {
		if (mask & 1)
			return shifts;
		mask = mask >> 1;
		shifts++;
	}
}

/**
 * intc_mask_irq
 *
 * Mask an IRQ in a bank
 *
 * @param irqn IRQ to mask
 */
void intc_unregister(int irqn) {
	if (irqn <= 7)
		*INTC_ENABLE_BASIC &= 0xFFFFFFFF - (1 << irqn);
}

/**
 * intc_unmask_irq
 *
 * Unmask an irq in a bank
 *
 * @param irqn IRQ to unmask
 */
void intc_register(int irqn, void* clear, void* handler) {
	intc_handlers[irqn][0] = clear; //Set the clear handler	
	intc_handlers[irqn][1] = handler; //Set the appropriate handler function pointer	
	if (irqn <= 7) { //Unmask the IRQ
		*INTC_ENABLE_BASIC |= (1 << irqn);
	}
}
