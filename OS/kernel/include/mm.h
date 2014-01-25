#ifndef MM_H
#define MM_H

void mm_init();
void mm_reserve(void* from, uintptr_t length);
void* mm_allocate(uintptr_t length);
void* mm_allocate_aligned(uintptr_t length, uintptr_t alignment);
void mm_free(void* address, uintptr_t length);

#endif
