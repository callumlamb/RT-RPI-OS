#include <stdint.h>
#include <stdbool.h>
#include <arch/mmu.h>
#include <arch/cpu.h>
#include <structures/bitmap.h>
#include <vm.h>
#include <heap.h>

//External symbols
extern uintptr_t _vkernel_start;
extern uintptr_t _vkernel_end;

//Kernel address space
static uint8_t kernel_buffer[MMU_PAGES(MMU_SPLIT) / 8];
static Bitmap kernel_bitmap = {&kernel_buffer, MMU_PAGES(MMU_SPLIT)};
vm_addr_space kernel_space = {&kernel_bitmap, (void*) MMU_SPLIT};

/*
 * virtual_init
 *
 * Create the kernels map
 */
void vm_init() {
	vm_reserve(&kernel_space, &_vkernel_start, &_vkernel_end - &_vkernel_start);
}

/*
 * vm_create_space
 */
vm_addr_space* vm_create_space() {

	//Allocate a page table
	//void* page_table = mmu_create_table();

	//Allocate a address space bitmap

	//TODO do in Bitmap?

	//Return vm_addr_space handle

	return 0;

}

/*
 * virtual_kernel_reserve
 *
 * Reserve a specific section of memory from the address space.
 */
void vm_reserve(vm_addr_space* space, void* address, uintptr_t length) {

	//Adjust address to go into the bitmap
	address -= (uintptr_t) space->base;

	//Get start and end pages
	uintptr_t start_page = MMU_PAGE_INDEX(address);
	uintptr_t end_page = start_page + MMU_PAGES(length);

	//Mark those bits as allocated
	bitmap_fill(space->bitmap, start_page, end_page);

}

/*
 * virtual_allocate
 *
 * Allocate a user specified size of the virtual address space
 */
void* vm_allocate(vm_addr_space* space, uintptr_t size) {

	//Get pages to allocate
	uintptr_t pages = MMU_PAGES(size);

	//Allocate the pages
	void* result = (void*) (bitmap_allocate(space->bitmap, pages)
			* MMU_PAGE_SIZE);

	//Adjust to actual address
	result += (uintptr_t) space->base;

	//Return pointer
	return (result);

}

/*
 * virtual_kernel_free
 *
 * Make the page available again
 */
void vm_free(vm_addr_space* space, void* address, uintptr_t length) {

	//Adjust address to go into the bitmap
	address -= (uintptr_t) space->base;

	//Get start and end pages
	uintptr_t start_page = MMU_PAGE_INDEX(address);
	uintptr_t end_page = start_page + MMU_PAGES(length);

	//Free those bits
	bitmap_clear(space->bitmap, start_page, end_page);

}

