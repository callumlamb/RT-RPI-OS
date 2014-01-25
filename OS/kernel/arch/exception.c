#include <stdint.h>
#include <arch/exception.h>
#include <syscall.h>
#include <arch/intc.h>
#include <scheduler.h>
#include <arch/cpu.h>
#include <arch/mmu.h>
#include <print.h>
#include <mm.h>
#include <vm.h>

//External symbols
extern vm_addr_space kernel_space;
extern uintptr_t page_table;

/*
 * exception_initialise
 *
 * Sets up the ARM ivt to stub routines that then redirect the exception to the appropriate handler in the kernel
 */
void exception_init() {

	//Enable the V bit in the c1 register to remap the vector table to 0xFFFF0000
	asm("\n\
			mov		r1, #1;\
			lsl		r1, r1, #13;\
			mrc		p15, 0, r0, c1, c0, 0;\
			orr		r0, r0, r1;\
			mcr		p15, 0, r0, c1, c0, 0;\
	");

	//Allocate 4kb of memory at vector table location 0xFFFF0000
	void* vectors = mm_allocate(0xFFFF);
	vm_reserve(&kernel_space, EXCEPTION_TABLE, 0xFFFF);
	mmu_map(&page_table, vectors, EXCEPTION_TABLE, 0xFFFF, WRITE_BACK);

	//Set each vector to load
	volatile uint32_t* ivt = EXCEPTION_TABLE;
	for(uint32_t i = 0; i < EXCEPTION_COUNT; i++) {

		//Encodes an LDR instruction that loads a pc relative address for an entry in a jump table immediatly following the vector table
		//Minus 8 because the offset counts from -8
		*ivt++ = 0xE59FF000 + (EXCEPTION_COUNT * 4 - 8);

	}

	//Set the default vector locations
	_exception_set_vector(EXCEPTION_DATA_ABORT, exception_data_abort);
	_exception_set_vector(EXCEPTION_IRQ, exception_irq);
	_exception_set_vector(EXCEPTION_SWI, exception_swi);

}

/*
 * exception_set_vector
 *
 * Set a vector to a certain pointer
 *
 * @param offset vector number to set
 * @param function pointer to function to call 
 */
static void _exception_set_vector(uint_fast8_t vector, void* addr) {

	//Get address of entry in jump table
	volatile uint32_t* entry = EXCEPTION_TABLE + (EXCEPTION_COUNT * 4) + (vector*4);

	//Set the entry to be the desired address
	*entry = (uintptr_t) addr;

}

/*
 * exception_irq
 *
 * Switch to system mode, save state, handle irq and load state
 */
void __attribute__((naked)) exception_irq() {
	asm("\n\
		sub	lr, lr, #4;\n\
		srsdb	#31!;\n\
		cps		#31;\n\
		stmfd	sp!, {r0-r12, r14};\n\
		mov		r0, sp;\n\
		ldr		r1, =scheduler_set_current;\n\
		blx		r1;\n\
		ldr		r0, =intc_handle;\n\
		blx		r0;\n\
		ldr		r0, =scheduler_get_current;\n\
		blx		r0;\n\
		mov		sp, r0;\n\
		ldmfd	sp!, {r0-r12, r14};\n\
		rfeia   sp!;\n\
	");
}

/*
 * exception_swi
 *
 * Gets the index function pointer from the syscall table and jumps to it.
 */
void __attribute__((naked)) exception_swi() {
	asm("\n\
		srsdb	#31!;\n\
		cps		#31;\n\
		cpsie	i;\n\
		cmp		r0, %0;\
		bgt		error;\
		handle:\
			ldr	r12, =syscall_table;\
			ldr	r12, [r12, r0, asl #2];\n\
			blx	r12;\n\
			rfeia	sp!;\n\
		error:\
			mov	r0, #-1;\
			rfeia	sp!;"
			:
			: "i" (SYSCALL_MAX)
			:
	);
	//TODO maybe do a sizeof the list to remove this constant?
}

/*
 * exception_data_abort
 *
 * Print some debug information
 */
void __attribute__((naked)) exception_data_abort() {
	asm("\n\
		mov		r0, lr;\
		sub     r0, #8;\
		ldr		r1, [r0];\
		l:b l;\
	");
}
