ENTRY(_start)

ram_vma = 0x30000000;
ram_size = 64 * 1024 * 1024;	/* 64M */

SECTIONS
{
	. = ram_vma;
	.text :
	{
		*(.text);
		*(.rodata)
	}
	
	.data : 
	{
		*(.data);
	}

	.bss :  
	{
		_bss_begin = .;
		*(.bss);
		. = ALIGN(1024);
		_bss_end = .;
	}

	_free_mem = ALIGN(1024);
}
