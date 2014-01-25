#include <stdint.h>
#include <arch/clock.h>

#define CLOCK_BASE	  		0xF2003000
#define CLOCK_TIMESTAMP 	((volatile uint32_t *) (CLOCK_BASE + 4))

/**
 * timer_get_time
 *
 * Get current timestamp from timer . Need to use inline assembly to read 64bits simultataneously
 * As C will consecutively load 2 dwords that could be out of sync.
 */
uint64_t clock_get_time() {
	uint64_t timestamp;
	asm volatile ("ldrd %0, [%1]" : "=r"(timestamp) : "r"(CLOCK_TIMESTAMP));
	return (timestamp);
}

/**
 * timer_wait
 *
 * Wait a specified time in microseconds. 
 */
void clock_wait(uint64_t delay) {
	uint64_t target = clock_get_time() + delay;
	while (clock_get_time() < target) {
		asm("nop");
	}
}

