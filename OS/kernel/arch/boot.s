/*
 * BSS Data
 */
.section ".bss"

	/* Stack */
	.skip	0x200
	stack:

	/* Page table */
	.align	14
	page_table:
	.globl 	page_table
	.skip	4096*4

	/* Coarse table */
	.align  10
	coarse_table:
	.globl coarse_table
	user_coarse_table:
	.skip	2048 * (256 * 4)
	kernel_coarse_table:
	.skip	2048 * (256 * 4)

/*
 * Entry point
 *
 * This section is actually mapped at mid-point but since the code is actually being run at the start
 * all symbols must have the offset deducted from them until the MMU is activated.
 */
.section ".text.boot"
entry:

	/* switch to system mode */
	cps		#31
	
	/* Calculate offset */
	mov		r12, #1
	lsl		r12, #31
	
	/* Set the stack */
	ldr		sp, =stack
	sub		sp, r12
		
	/* Clear bss */
	bl		clear_bss

	/* Map sections */
	ldr		r1,	=page_table /* Get pointer to user and kernel tables */
	sub		r1,	r12	
	ldr		r4, =coarse_table /* Set first entries pointer */
	sub     r4, r12
	add		r4, #0x1	
	mov		r5, #0
	mov		r3, #0x1000
	.sloop:		/* Set coarse page table pointer */
		str		r4, [r1]
		add		r1, #4
		add		r4, #0x400
		add		r5, #1
		cmp		r5, r3
		blt		.sloop	

	/* Map pages */
	ldr		r1,	=user_coarse_table /* Get pointer to user and kernel tables */
	sub		r1,	r12
	ldr		r2,	=kernel_coarse_table
	sub		r2, r12
	ldr		r4, =_pkernel_start /* Set first entries pointer */
	add		r4, #0xE
	ldr		r3, =_kernel_pages /* Get count of sections to set */
	mov		r5, #0
	.cloop:		/* Set each section entry */
		str		r4, [r1]
		str		r4, [r2]
		add		r1, #4
		add		r2, #4 
		add		r4, #0x1000
		add		r5, #1
		cmp		r5, r3
		blt		.cloop
		
	/*Enable MMU*/
	bl		enable_mmu

	/* Set the stack to the higher location */
	ldr		sp, =stack	
	
	/*Jump to main */
	ldr		r4, =main
	blx		r4

/*
 * enable_mmu
 *
 * Configure all the MMU registers, clear entries and turn on the MMU.
 */
enable_mmu:

	/*Set MMU to use sep kernel and user tables */
	mov		r0, #1
	mcr 	p15, 0, r0, c2, c0, 2

	/*Set section_table to the kernel page table and the user page table (Second 2GB points to first 2GB, and vice versa)*/
	ldr		r0, =page_table
	sub		r0, r12		
	add		r0, #0x19		/*Turn on inner and outer caching of the page table*/
	mcr		p15, 0, r0, c2, c0, 0   /*Set user table*/
	mcr		p15, 0, r0, c2, c0, 1	/*Set kernel table*/

	/*Set access control to all-supervisor*/
	mov		r1, #0
	mvn		r0, r1
	mcr		p15, 0, r0, c3, c0, 0

	/*Disable subpage AP bits*/
	mrc		p15, 0, r0, c1, c0, 0
	mov		r1, #0
	lsl		r1, r1, #23
	orr		r0, r1
	mcr		p15, 0, r0, c1, c0, 0

	/*Invalidate all TLB entries*/
	mcr		p15, 0, r0, c8, c7, 0

	/*Turn on the MMU*/
	mrc		p15, 0, r0, c1, c0, 0
	orr		r0, #1
	mcr		p15, 0, r0, c1, c0, 0

	/*Enable MMU dependant optimizations*/
	mrc		p15, 0, r0, c1, c0, 0
	mov		r1, #3
	lsl		r1, r1, #11
	add		r1, #4
	orr		r0, r1
	mcr		p15, 0, r0, c1, c0, 0

	/*Return*/
	bx		lr

/*
 * clear_bss
 *
 * Clears the BSS section. O3 compiler generated code for armv6j.
 */
clear_bss:
	ldr		r0, =_bss_end
	sub		r0, r12
	ldr		r4, =_bss_start
	sub		r4, r12
	cmp		r4, r0
	bcs		.finish
	rsb		r5, r4, r0
	mov		r1, r5, lsr #2
	cmp		r1, #0
	cmpne	r5, #3
	movhi	r3, #0
	movls	r3, #1
	mov		r6, r1, asl #2
	bls		.start
	sub		r2, r4, #4
	mov		r7, r3
	.loop:
		add		r3, r3, #1
		cmp		r1, r3
		str		r7, [r2, #4]!
		bhi		.loop
		cmp		r5, r6
		add		r3, r4, r6
		beq		.finish
	.clear:
		mov		r2, #0
	.end:
		strb	r2, [r3], #1
		cmp		r3, r0
		bcc		.end
	.finish:
		bx		lr
	.start:
		mov		r3, r4
		b		.clear
