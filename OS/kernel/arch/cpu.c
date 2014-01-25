#include <stdint.h>

/*
 * cpu_memset
 *
 * memset implementation
 */
void cpu_memset(void* address, uint8_t value, uintptr_t length) {
	for (uint8_t* i = address; (uintptr_t) i < (uintptr_t) address + length;
			i++) {
		*i = value;
	}
}

/*
 * cpu_memzero
 *
 * memzero implementation
 */
void cpu_memzero(void* address, uintptr_t length) {
	for (uint8_t *i = address; (uintptr_t) i < (uintptr_t) address + length;
			i++) {
		*i = 0;
	}
}

/*
 * cpu_memfill
 *
 * memfill implementation
 */
void cpu_memfill(void* address, uintptr_t length) {
	for (uint8_t *i = address; (uintptr_t) i < (uintptr_t) address + length;
			i++) {
		*i = 0xFF;
	}
}

/*
 * cpu_memcpy
 *
 * memcpy implementation
 */
void cpu_memcpy(void* source, void* destination, uintptr_t length) {
	uint8_t* from = source;
	for (uint8_t *i = destination;
			(uintptr_t) i < (uintptr_t) destination + length; i++) {
		*i = *from++;
	}
}
