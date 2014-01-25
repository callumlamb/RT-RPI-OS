#include <stdint.h>
#include <stdbool.h>
#include <arch/framebuffer.h>
#include <arch/mmu.h>
#include <mm.h>
#include <vm.h>

//External symbols
extern uintptr_t page_table;
extern vm_addr_space kernel_space;

//Framebuffer structure
static volatile FrameBufferDescription frameBufferInfo = {
FRAMEBUFFER_WIDTH, //Width
		FRAMEBUFFER_HEIGHT, //Height
		FRAMEBUFFER_WIDTH, //vWidth
		FRAMEBUFFER_HEIGHT, //vHeight
		0, 16, //Bits
		0, 0, 0, 0 };

//Pointer to framebuffer
void* fb;

/*
 * framebuffer_get
 */
void* framebuffer_get() {
	return (fb);
}

/**
 * framebuffer_write
 *
 * Write to the framebuffer.
 *
 * @param value the value to send, lowest 4 bits must be empty
 */
static void _framebuffer_write(uint32_t value) {
	while ((*MAILBOX_STATUS & MAILBOX_WRITE_READY) != 0)
		;	//Wait until mailbox status indicates it's ready to send mail
	*MAILBOX_WRITE = value + 1;	//Send mail with GPU channel in the lowest 4 bits
}

/**
 * framebuffer_read
 *
 * Read back from the framebuffer.
 *
 * @return The mail from the GPU
 */
static uint32_t _framebuffer_read() {
	while ((*MAILBOX_STATUS & MAILBOX_READ_READY) != 0)
		;	 //Wait until status indicates the MAILBOX has mail ready
	uint32_t mail; //Keep getting mail until the mail for channel 1 is recieved
	do {
		mail = *MAILBOX_READ;
	} while ((mail & 0xF) != 1);
	return (mail & 0xFFFFFFF0); //Return the mail (with the channel masked out)
}

/**
 * framebuffer_initialise        
 *
 * Setup the framebuffer settings and framebuffer
 */
void framebuffer_init() {

	//Get a pointer to the framebuffer
	void* pFbInfo = mmu_get_physical(&page_table, (void*) &frameBufferInfo);
	_framebuffer_write((uintptr_t) pFbInfo | MAILBOX_WRITE_CLEAR_CACHE);
	_framebuffer_read();
	while (frameBufferInfo.pointer == 0)
		;

	//Reserve and map framebuffer into kernel memory
	mm_reserve(frameBufferInfo.pointer, frameBufferInfo.size);
	fb = vm_allocate(&kernel_space, frameBufferInfo.size);
	mmu_map(&page_table, (void*) frameBufferInfo.pointer, fb,
			frameBufferInfo.size, CACHE_DISABLED); //Map in at the request virtual address

}
