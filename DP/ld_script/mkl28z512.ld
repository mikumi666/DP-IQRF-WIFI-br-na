/*
 *==============================================================================
 * Name: mkl28z512.ld
 * Edit: Martin Stankus
 * 
 * ld docs: https://sourceware.org/binutils/docs/ld/index.html
 *==============================================================================
 */
 
/* stack size shall be a multiple of 8 */
STACK_SIZE_DEF = 0x800;
STACK_SIZE_VAL = DEFINED(STACK_SIZE) ? STACK_SIZE : STACK_SIZE_DEF;

MEMORY
{
	m_flash_vectab	(R) : ORIGIN = 0x00000000, LENGTH = 0x00000140 /* 320 B */
	m_flash_gap		(R) : ORIGIN = 0x00000140, LENGTH = 0x000002C0 /* 704 B */
	m_flash_fcf		(R) : ORIGIN = 0x00000400, LENGTH = 0x00000010 /* 16 B */
	m_flash_bulk	(RX) : ORIGIN = 0x00000410, LENGTH = 0x0007FBF0 /* 512 kB - 320 B - 704 B - 16 B */
	m_sram			(RW) : ORIGIN = 0x1FFF8000, LENGTH = 0x00020000 /* 128 kB */
	m_usb_data		(RW) : ORIGIN = 0x40100000, LENGTH = 0x00000800	/* 2K bytes */
}

SECTIONS
{
	.vectab :
	{
		KEEP(*(.vectab))
	} > m_flash_vectab
	
	.fcf :
	{
		KEEP(*(.fcf))
	} > m_flash_fcf
	
	.text :
	{
		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
	} > m_flash_bulk
		
	.stack :
	{
		. = ALIGN(8);
		STACK_BOTTOM = .;
		. += STACK_SIZE_VAL;
		. = ALIGN(8);
		STACK_TOP = .;
	} > m_sram
		
	.data : ALIGN_WITH_INPUT
	{
		DATA_BEGIN = .;
		*(.data)
		*(.data*)
		*(.data_RAM2)
		*(.data_RAM2*)
		DATA_END = .;
	} > m_sram AT> m_flash_bulk
	
	DATA_INIT = LOADADDR(.data);
	
	.bss :
	{
		BSS_BEGIN = .;
		*(.bss)
		*(.bss*)
		*(.bss_RAM2)
		*(.bss_RAM2*)
		*(COMMON)
		BSS_END = .;
	} > m_sram
	
    .m_usb_data (NOLOAD) :
    {
        *(m_usb_bdt)
        *(m_usb_global)
    } > m_usb_data
    
    .mtb_buffer_RAM2 (NOLOAD) :
    {
        KEEP(*(.mtb.$RAM2*))
        KEEP(*(.mtb.$USB_RAM*))
    } > m_usb_data

	.data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        *(.ramfunc.$RAM2)
        *(.ramfunc.$USB_RAM)
        *(.data.$RAM2*)
        *(.data.$USB_RAM*)
        . = ALIGN(4) ;
     } > m_usb_data AT> m_sram

	bss_RAM2 : ALIGN(4)
    {
       *(.bss.$RAM2*)
       *(.bss.$USB_RAM*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
    } > m_usb_data 
    
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM2*)
       *(.noinit.$USB_RAM*)
       . = ALIGN(4) ;
    } > m_usb_data 
}
