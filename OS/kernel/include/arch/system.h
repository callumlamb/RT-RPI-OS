#ifndef SYSTEM_H
#define SYSTEM_H

//Constants
#define MMAP_MAX	10

//ATAG_MEM entry
typedef struct {
	uint32_t size; /* size of the area */
	uint32_t start; /* physical start address */
} mmap_entry;

//prototypes
void system_init();
mmap_entry* system_mmap();
void __attribute__((naked)) system_load_state(void* context);
void* system_create_state(void* entry);

#endif
