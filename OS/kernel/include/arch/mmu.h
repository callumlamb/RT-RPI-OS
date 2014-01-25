#ifndef MMU_H
#define MMU_H

//Page cache settings
typedef enum {
	CACHE_DISABLED = 2,
	WRITE_THROUGH = 0xA,
	WRITE_BACK = 0xE
} page_type;

/*
 * Prototypes
 */

void mmu_init();
void* mmu_create_table();
void mmu_switch_table(void* table);
void mmu_free_table(void* table);
void mmu_map(void* table, void* physical, void* virtual, uintptr_t length,
		page_type type);
void* mmu_get_physical(void* table, void* virtual);
void mmu_unmap(void* table, void* virtual, uintptr_t length);

/*
 * Config
 */

//MMU configuration
#define MMU_MAX_ADDR			0x100000000
#define MMU_MAX_PHYSICAL		0x80000000
#define MMU_SPLIT				0x80000000
#define MMU_SECTION_SIZE		0x100000
#define MMU_PAGE_SIZE			0x1000

//First level table constants
#define FIRST_TABLE_SIZE		0x4000
#define FIRST_TABLE_ENTRIES		0x1000
#define FIRST_TABLE_FLAGS		0x1
#define FIRST_TABLE_MASK		0x3FF

//Second level table constants
#define SECOND_TABLE_SIZE		0x400
#define SECOND_TABLE_ENTRIES	0x100
#define SECOND_TABLE_FLAGS		0xE
#define SECOND_TABLE_MASK		0xFFF

/*
 * Helper macros
 */

#define MMU_PAGES(x) ((((uintptr_t) x) + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE)
#define MMU_PAGE_UP(x) ((void*)(((((uintptr_t) x) + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE) * MMU_PAGE_SIZE))
#define MMU_PAGE_DOWN(x) ((void*)((((uintptr_t) x) / MMU_PAGE_SIZE) * MMU_PAGE_SIZE))
#define MMU_PAGE_INDEX(x) (((uintptr_t) x) / MMU_PAGE_SIZE)

#endif
