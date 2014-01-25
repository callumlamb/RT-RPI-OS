#include <stdint.h>
#include <heap.h>
#include <vm.h>
#include <mm.h>
#include <arch/mmu.h>

//external variables
extern uintptr_t page_table;
extern vm_addr_space kernel_space;

/*
 * heap_alloc
 *
 * Allocate memory on the heap
 */
void* heap_alloc(uintptr_t size) {
	void* physical = mm_allocate(size);
	void* virtual = vm_allocate(&kernel_space, size);
	mmu_map(&page_table, physical, virtual, size, WRITE_BACK);
	return (virtual);
}

/*
 * heap_free
 * 
 * Free memory from the heap
 */
void heap_free(void* buffer, uintptr_t size) {
	vm_free(&kernel_space, buffer, size);
	mm_free(buffer, size);
	mmu_unmap(&page_table, buffer, size);
}
