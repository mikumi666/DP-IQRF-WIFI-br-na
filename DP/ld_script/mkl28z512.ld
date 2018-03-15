/*
 *==============================================================================
 * Name: mkl28z512.ld
 * Author: Martin Stankus
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
		DATA_END = .;
	} > m_sram AT> m_flash_bulk
	
	DATA_INIT = LOADADDR(.data);
	
	.bss :
	{
		BSS_BEGIN = .;
		*(.bss)
		*(.bss*)
		*(COMMON)
		BSS_END = .;
	} > m_sram
}
