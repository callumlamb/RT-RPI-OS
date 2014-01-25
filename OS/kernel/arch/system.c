#include <stdint.h>
#include <arch/system.h>
#include <mm.h>
#include <vm.h>
#include <arch/mmu.h>
#include <heap.h>
#include <arch/framebuffer.h>

//external symbols
extern uintptr_t page_table;
extern vm_addr_space kernel_space;

//Prototypes
static void _system_map_hardware();

/*
 * ATAGS
 */

//memory map buffer
mmap_entry mmap[MMAP_MAX];

//ATAGS
#define ATAG_BASE	0x100
#define ATAG_NONE	0
#define ATAG_MEM	0x54410002

//ATAG header
typedef struct {
	uint32_t size; /* legth of tag in words including this header */
	uint32_t tag; /* tag value */
} atag_header;

/*
 * MMIO regions
 */

#define IO_PHYSICAL		((void*)0x20000000)
#define IO_VIRTUAL		((void*)0xF2000000)
#define IO_LENGTH		0xFFFFFF

/*
 * system_init
 *
 * Initialises the systems IO regions and chipset hardware
 */
void system_init() {
	_system_map_hardware(); //Map IO region	
	framebuffer_init(); //Initialise hardware
}

/*
 * _system_map_hardware
 *
 * Map hardware MMIO regions and non-memory areas into the kernels virtual address space
 */
static void _system_map_hardware() {
	mm_reserve(IO_PHYSICAL, IO_LENGTH);
	vm_reserve(&kernel_space, IO_VIRTUAL, IO_LENGTH);
	mmu_map(&page_table, IO_PHYSICAL, IO_VIRTUAL, IO_LENGTH, CACHE_DISABLED);
}

/*
 * system_load_state
 *
 * Switch to the given state
 */
void __attribute__((naked)) system_load_state(void* context) {
	asm("\
		mov	sp, %0;\n\
		ldmfd	sp!, {r0-r12, r14};\n\
		rfeia   sp!;"
			:
			: "r" ((uint32_t)context)
			:
	);
}

/*
 * system_create_state
 */
void* system_create_state(void* entry) {
	uint32_t* stack = heap_alloc(512) + 512; //Allocate a stack
	*stack-- = 16;   //CPSR		
	*stack-- = (uint32_t) entry; //Initial PC of process
	stack -= 13;
	return (stack);
}

/*
 * system_mmap
 *
 * Generate a map of memory
 */
mmap_entry* system_mmap() {
	atag_header* current_atag = (atag_header*) ATAG_BASE; //Pointer to first atag, TODO hardcoded atags address
	mmap_entry* entry;
	int current = 0;
	while (current_atag->tag != ATAG_NONE) { //Parse each memory tag until ATAG_NONE is reached
		switch (current_atag->tag) {
		case ATAG_MEM: //If a mem tag the entry to the mmap
			entry = (void*) current_atag + sizeof(atag_header); //Get pointer to atag_mem
			mmap[current++] = *entry; //Set the entry
		default: //Otherwise go to next tag
			current_atag = (atag_header*) ((uint32_t*) current_atag
					+ current_atag->size);
			break;
		}
	}
	return ((mmap_entry*) mmap);
}
