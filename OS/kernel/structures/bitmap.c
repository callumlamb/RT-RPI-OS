#include <stdint.h>
#include <stdbool.h>
#include <structures/bitmap.h>

/*
 * bitmap_get
 *
 * Gets the status of a bit in the bitmap
 */
bool bitmap_get(Bitmap* bitmap, uintptr_t index) {
	uint8_t* byte = bitmap->start + (index / 8);
	if (*byte & 1 << (index % 8))
		return (true);
	else
		return (false);
}

/*
 * bitmap_set
 *
 * Gets the status of a bit in the bitmap
 */
void bitmap_set(Bitmap* bitmap, uintptr_t index) {
	uint8_t* byte = bitmap->start + (index / 8);
	*byte |= 1 << (index % 8);
}

/*
 * bitmap_unset
 *
 * Clear a bit
 */
void bitmap_unset(Bitmap* bitmap, uintptr_t index) {
	uint8_t* byte = bitmap->start + (index / 8);
	*byte &= ~(1 << (index % 8));
}

/*
 * bitmap_fill
 *
 * Set a region of the bitmap
 */
void bitmap_fill(Bitmap* bitmap, uintptr_t start, uintptr_t end) {
	if (start > bitmap->length || start > end)
		return;
	else if (end > bitmap->length)
		end = bitmap->length;
	for (uintptr_t i = start; i < end; i++)
		bitmap_set(bitmap, i);
}

/*
 * bitmap_clear
 *
 * Clear a region of the bitmap
 */
void bitmap_clear(Bitmap* bitmap, uintptr_t start, uintptr_t end) {
	if (start > bitmap->length || start > end)
		return;
	else if (end > bitmap->length)
		end = bitmap->length;
	for (uintptr_t i = start; i < end; i++)
		bitmap_unset(bitmap, i);
}

/*
 * bitmap_find_unset
 *
 * Find count unset bits with an alignment of alignment
 */
uintptr_t bitmap_allocate(Bitmap* bitmap, uintptr_t count) {
	return (bitmap_allocate_aligned(bitmap, count, 0));
}

/*
 * bitmap_find_unset
 *
 * Find count unset bits with an alignment of alignment
 */
uintptr_t bitmap_allocate_aligned(Bitmap* bitmap, uintptr_t count,
		uintptr_t page_align) {
	for (uintptr_t i = 0; i < bitmap->length; i++) {	//Loop through each bit	
		if (i % (1 << page_align) == 0 && !bitmap_get(bitmap, i)) {	//If bit is clear	
			bool big_enough = true; //Check to see if it's big enough
			for (uintptr_t j = 1; j < count; j++) {
				if (bitmap_get(bitmap, i + j))
					big_enough = false;
			}
			if (big_enough) { //If so set all the bits and return the pointer
				for (uintptr_t j = 0; j < count; j++)
					bitmap_set(bitmap, i + j);
				return (i);
			}
		}
	}
	return (0);
}

