#ifndef VIRTUAL_H
#define VIRTUAL_H

//Includes
#include "structures/bitmap.h"

//Address space structure
typedef struct {
	Bitmap* bitmap;
	void* base;
} vm_addr_space;

//Prototypes
void vm_init();
vm_addr_space* vm_create_space();
void vm_reserve(vm_addr_space* space, void* address, uintptr_t length);
void* vm_allocate(vm_addr_space* space, uintptr_t length);
void vm_free(vm_addr_space* space, void* address, uintptr_t length);

#endif
