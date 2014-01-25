#include <stdint.h>
#include <stdbool.h>
#include <arch/cpu.h>
#include <mm.h>
#include <arch/mmu.h>
#include <structures/bitmap.h>
#include <arch/system.h>

//External symbols
extern uintptr_t _pkernel_start;
extern uintptr_t _pkernel_end;

//Bitmap buffer
static uint8_t buffer[MMU_PAGES(MMU_MAX_PHYSICAL) / 8];
static Bitmap bitmap = { &buffer, MMU_PAGES(MMU_MAX_PHYSICAL) };

/*
 * memory_init
 *
 * Initialise memory manager
 */
void mm_init() {

	//Apply memory map to memory bitmap
	mmap_entry* mmap = system_mmap(); 	//Get memory map
	mm_reserve(0, bitmap.length); //Mark everything as allocated
	while (mmap->size != 0) { //Unallocate usable memory
		mm_free((void*) mmap->start, mmap->size);
		mmap++;
	}

	//Remove the kernel pages from the bitmap
	mm_reserve(&_pkernel_start, &_pkernel_end - &_pkernel_start);

}

/*
 * memory_allocate
 *
 * Allocate a page and return its pointer
 */
void* mm_allocate(uintptr_t length) {
	uintptr_t count = MMU_PAGES(length); //Get length in pages	
	return ((void*) (bitmap_allocate(&bitmap, count) * MMU_PAGE_SIZE)); //Return pointer
}

/*
 * memory_allocate_aligned
 *
 * Allocate a page and return its pointer
 */
void* mm_allocate_aligned(uintptr_t length, uintptr_t page_align) {
	uintptr_t count = MMU_PAGES(length); //Get length in pages	
	return ((void*) (bitmap_allocate_aligned(&bitmap, count, page_align)
			* MMU_PAGE_SIZE)); //Return pointer
}

/*
 * memory_reserve
 *
 * Reserve memory from being allocated
 */
void mm_reserve(void* from, uintptr_t length) {
	bitmap_fill(&bitmap, MMU_PAGE_INDEX(from),
	MMU_PAGE_INDEX(from) + MMU_PAGES(length));
}

/*
 * memory_free
 *
 * Free memory
 */
void mm_free(void* address, uintptr_t length) {
	bitmap_clear(&bitmap, MMU_PAGE_INDEX(address),
	MMU_PAGE_INDEX(address) + MMU_PAGES(length));
}
