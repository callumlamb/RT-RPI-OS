#include <stdint.h>
#include <arch/mmu.h>
#include <mm.h>

//Linker symbols
extern uintptr_t coarse_table;

//TODO use not global property to specify process pages and tag them with current ASID
//TODO on each page table switch invalidate all the previous ASID's pages

/*
 * mmu_init
 */
void mmu_init() {

	//allocate page(s) to use as scratch space



}

/*
 * mmu_create_table
 *
 * Create a new address space
 */
void* mmu_create_table() {
	return (mm_allocate_aligned(FIRST_TABLE_SIZE, 14));

	//TODO clear the memory

}

/*
 * mmu_switch_table
 *
 * Switch address space
 */
void mmu_switch_table(void* table) {

	//Enable inner and outer caching
	table = (void*) ((uintptr_t) table | 0x19);

	//Switch the table
	asm("mcr p15, 0, %0, c2, c0, 0"::"r" (table));

	//Invalidate table entries
	asm("mcr p15, 0, r0, c8, c7, 0");

}

/*
 * mmu_delete_table
 */
void mmu_delete_table(void* table) {
	//TODO iterate through each coarse page table deallocating that too
	//memory_free(table, SECTION_TABLE_SIZE);
	(void) (table);
}

/*
 * mmu_map_page
 * 
 * Map a page
 *
 */
static void _mmu_map_page(void* table, void* physical, void* virtual,
		page_type type) {

	//Calculate section index of virtual address
	uintptr_t section_index = MMU_PAGE_INDEX(virtual) / SECOND_TABLE_ENTRIES;

	//Get pointer to first level table entry
	volatile uint32_t* first = table;
	first += section_index;

	//If first  level entry is marked as unallocated then allocate it
	if (!(*first & 1)) {

		//Create a new second level table
		void* new_table = mm_allocate(SECOND_TABLE_SIZE);

		//Map ourselves, since the kernel page tables are virtually allocated in it we shouldnt go more than 2 levels deep
		mmu_map(table, new_table,
				MMU_PAGE_DOWN(
						&coarse_table + (section_index * SECOND_TABLE_SIZE)),
				SECOND_TABLE_SIZE, type);

		//Allocate a secondary table
		*first = (uintptr_t) new_table | FIRST_TABLE_FLAGS;

	}

	//Set the entry in the second page table
	volatile uint32_t* entry = (uint32_t*) (*first & ~FIRST_TABLE_MASK);
	uint32_t page_index = (MMU_PAGE_INDEX(virtual) % SECOND_TABLE_ENTRIES);
	entry += page_index;

	//map in the page
	*entry = (uint32_t) physical | type;

}

/*
 * mmu_map
 *
 * Map a region of memory
 *
 */
void mmu_map(void* table, void* physical, void* virtual, uintptr_t length,
		page_type type) {

	//Loop through each page mapping required
	for (uintptr_t i = 0; i < MMU_PAGES(length); i++) {
		_mmu_map_page(table, physical + (i * MMU_PAGE_SIZE),
				virtual + (i * MMU_PAGE_SIZE), type);
	}

	//Invalidate table entries
	asm("mcr p15, 0, r0, c8, c7, 0");

}

/*
 * mmu_unmap
 * 
 * Unmap a page
 */
void mmu_unmap(void* table, void* virtual, uintptr_t length) {

	(void) (table);
	(void) (virtual);
	(void) (length);

	//TODO

}

/*
 * mmu_get_physical
 *
 * Get physical address of a virtual address
 */
void* mmu_get_physical(void* table, void* virtual) {
	uintptr_t page = MMU_PAGE_INDEX(virtual); //Convert address to a page index
	uintptr_t first_offset = (page / 0x100) * 4; //Get offset into first level page table	
	uint32_t* first = table + first_offset; //Get pointer to first page entry	
	uint32_t* second = (void*) (*first & ~(FIRST_TABLE_MASK)); //Get address of the second level table	
	uint32_t* entry = second + (page % 0x100); //Get address of the second level table entry	
	void* addr = (void*) (*entry & ~(SECOND_TABLE_MASK)); //Get address of second level entry	
	addr += (uintptr_t) virtual % MMU_PAGE_SIZE; //Add on the remainder	
	return (addr); //Return the physical address
}
