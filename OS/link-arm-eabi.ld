ENTRY(_entry)
SECTIONS
{
	
	/* entry */
	_entry = 0x80010000;
	. = 0x80010000;

	/* Start */
	_pkernel_start = 0x0;
	_vkernel_start = 0x80000000;	

	/* Sections */
	.text.boot : {*(.text.boot)}
	. = ALIGN(4096);
    .text : {*(.text)}
    . = ALIGN(4096);
    .rodata : {*(.rodata)}
    . = ALIGN(4096);
    .data : {*(.data)}
    . = ALIGN(4096);	
    .bss : {_bss_start = .;*(.bss)_bss_end = .;}

    /* End */
    _pkernel_end = . - 0x80000000;
    _vkernel_end = .;

    /* Kernel section count */
    _kernel_pages = (_pkernel_end - _pkernel_start + 0x1000 - 1) / 0x1000;

}
