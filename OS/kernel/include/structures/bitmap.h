#ifndef BITMAP_H
#define BITMAP_H

//Includes
#include <stdbool.h>

//Bitmap
typedef struct {
	void* start;
	uintptr_t length;
} Bitmap;

//Prototypes
bool bitmap_get(Bitmap* bitmap, uintptr_t index);
void bitmap_set(Bitmap* bitmap, uintptr_t index);
void bitmap_unset(Bitmap* bitmap, uintptr_t index);
void bitmap_fill(Bitmap* bitmap, uintptr_t start, uintptr_t end);
void bitmap_clear(Bitmap* bitmap, uintptr_t start, uintptr_t end);
uintptr_t bitmap_allocate_aligned(Bitmap* bitmap, uintptr_t count,
		uintptr_t alignment);
uintptr_t bitmap_allocate(Bitmap* bitmap, uintptr_t count);

#endif
