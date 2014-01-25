#include <stdint.h>
#include <arch/timer.h>
#include <arch/exception.h>
#include <arch/intc.h>
#include <arch/system.h>
#include <arch/cpu.h>
#include <arch/keyboard.h>
#include <scheduler.h>
#include <print.h>
#include <mm.h>
#include <syscall.h>
#include <vm.h>

#include <arch/mmu.h>

void A();
void B();

//TODO
//Print actual memory available
//framebuffer and the entire IO region should be mapped as noncacheable
//sep address spaces with each having the correct privileges

/**
 * main
 *
 * Entry point
 */
void main() {

	//Setup memory management
	mm_init();
	vm_init();

	//Initialise the system, configures the hardware and maps in relevant hardware memory regions
	//TODO change all the cache/branch prediction settings in here
	system_init();

	//Print startup messages
	print("System started.\n");

	//Register exceptions
	exception_init();

	//Initialise keyboard
	//keyboard_init();

	//Add 2 processes to the scheduler
	scheduler_add(B);

	//Start the scheduler
	timer_initialise(SCHEDULER_FREQUENCY); //Set timer interval	
	intc_register(TIMER_IRQ, timer_clear_interrupt, scheduler_next); //Register scheduler_next on the TIMER IRQ

	//mmu_switch_table((void*) 0);

	scheduler_add(A); //Add the command line process to the scheduler
	CPU_INTERRUPTS_ENABLE();

	//Load the first processes state
	system_load_state(scheduler_get_current());

}

/*
 * A process
 */
void A() {
	//Call the operating system print service
	while (1) {
		char* s_a = "A";
		asm("\
			mov	r0, %0;\
			mov	r1, #1;\
			mov	r2, %1;\
			mov	r3, %2;\
			svc	#0;"
				:
				: "i" (SYSCALL_WRITE), "r" (s_a), "r" (sizeof s_a)
				: "r0", "r1", "r2", "r3", "r12");
	}
}

/*
 * B process
 */
void B() {
	while (1) {
		char* s_b = "B";
		asm("\
			mov	r0, %0;\
			mov	r1, #1;\
			mov	r2, %1;\
			mov	r3, %2;\
			svc	#0;"
				:
				: "i" (SYSCALL_WRITE), "r" (s_b), "r" (sizeof s_b)
				: "r0", "r1", "r2", "r3", "r12");
	}
}
