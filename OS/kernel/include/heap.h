#ifndef HEAP_H
#define HEAP_H

void* heap_alloc(uintptr_t size);
void heap_free(void* buffer, uintptr_t size);

#endif
