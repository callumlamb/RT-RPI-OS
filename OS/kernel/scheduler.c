#include <stdint.h>
#include <print.h>
#include <scheduler.h>
#include <arch/system.h>

//Variables
void* scheduler_queue[10];			//TODO allocate this on the heap
unsigned int scheduler_current = 0;		//Current index in the queue
unsigned int scheduler_count = 0;		//Total count of processes

/**
 * scheduler_next
 *
 * Switch to next process to be run
 */
void scheduler_next() {
	if (++scheduler_current == scheduler_count)
		scheduler_current = 0; //Switch to next task
}

/*
 * scheduler_get_current
 *
 * Get current state pointer
 */
void* __attribute__((used)) scheduler_get_current() {
	return (scheduler_queue[scheduler_current]);
}

/*
 * scheduler_set_current
 *
 * Set current tasks stack pointer
 */
void __attribute__((used)) scheduler_set_current(void* context) {
	scheduler_queue[scheduler_current] = context; //Set current context
}

/**
 * scheduler_add
 *
 * Add another process to the queue
 */
void scheduler_add(void* entry) {

	//Print an error if too many processes
	if ((scheduler_count + 1) > sizeof(scheduler_queue)) {
		print("Maximum process count exceeded!\n");
		return;
	}

	//Create a context	
	void* context = system_create_state(entry);

	//Add it to the queue
	scheduler_queue[scheduler_count] = context;
	scheduler_count++;

}
