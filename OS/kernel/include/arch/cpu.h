#ifndef CPU_H
#define CPU_H

/*
 * Prototypes
 */

void cpu_memset(void* address, uint8_t value, uintptr_t length);
void cpu_memzero(void* address, uintptr_t length);
void cpu_memcpy(void* source, void* destination, uintptr_t length);
void cpu_memfill(void* address, uintptr_t length);

/**
 * CPU_INTERRUPTS_ENABLE
 *
 * Clears the interrupt mask flag is cpsr
 */
static inline void CPU_INTERRUPTS_ENABLE() {
	asm("cpsie i");
}

/**
 * CPU_INTERRUPTS_DISABLE
 *
 * Sets the interrupt mask flag is cpsr
 */
static inline void CPU_INTERRUPTS_DISABLE() {
	asm("cpsid i");
}

#endif
