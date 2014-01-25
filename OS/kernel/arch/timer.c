#include <stdint.h>
#include <arch/timer.h>

#define TIMER_BASE 					0xF200B000
#define TIMER_LOAD					((volatile uint32_t *) (TIMER_BASE + 0x400))
#define TIMER_VALUE					((volatile uint32_t *) (TIMER_BASE + 0x404))
#define TIMER_CTRL					((volatile uint32_t *) (TIMER_BASE + 0x408))
#define TIMER_ACK					((volatile uint32_t *) (TIMER_BASE + 0x40C))
#define TIMER_RAW_IRQ				((volatile uint32_t *) (TIMER_BASE + 0x410))
#define TIMER_MASK_IRQ				((volatile uint32_t *) (TIMER_BASE + 0x414))
#define TIMER_RELOAD				((volatile uint32_t *) (TIMER_BASE + 0x418))

#define TIMER_CTRL_23BIT			(1 << 1)
#define TIMER_CTRL_INT_ENABLE		(1 << 5)
#define TIMER_CTRL_ENABLE			(1 << 7)

/**
 * timer_initialise
 *
 * Setup the timer to periodically interrupt in order to make a scheduler.
 */
void timer_initialise(uint32_t delay) {

	//Configure timer
	*TIMER_CTRL = TIMER_CTRL_INT_ENABLE | TIMER_CTRL_ENABLE | TIMER_CTRL_23BIT;

	//Set counter
	*TIMER_LOAD = delay;

}

/**
 * timer_clear_interrupt()
 *
 * Clears the source of the interrupt
 */
void timer_clear_interrupt() {

	//Write anything to the TIMER_ACK register
	*TIMER_ACK = 1;

}
