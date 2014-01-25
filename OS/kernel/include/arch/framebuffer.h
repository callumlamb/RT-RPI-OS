#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

//Framebuffer description
typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t vWidth;
	uint32_t vHeight;
	uint32_t pitch;
	uint32_t bitDepth;
	uint32_t x;
	uint32_t y;
	uint16_t* pointer;
	uint32_t size;
}__attribute__((aligned(16))) FrameBufferDescription;

//framebuffer settings
#define FRAMEBUFFER_WIDTH 	1024
#define FRAMEBUFFER_HEIGHT 	768

//Mailbox settings
#define MAILBOX_WRITE_READY 		0x80000000
#define MAILBOX_READ_READY  		0x40000000
#define MAILBOX_WRITE_CLEAR_CACHE 	0x40000000
#define MAILBOX_BASE   				0xF200B880
#define MAILBOX_READ   				((volatile uint32_t *) (MAILBOX_BASE))
#define MAILBOX_STATUS 				((volatile uint32_t *) (MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE  				((volatile uint32_t *) (MAILBOX_BASE + 0x20))

//Functions
void framebuffer_init();
void* framebuffer_get();

#endif
