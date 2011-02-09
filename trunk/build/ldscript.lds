ENTRY(_start)
bootstone_vma = 0x0;
bootstone_size = 4 * 1024;		/* 4K */

ram_vma = 0x30000000;
ram_size = 64 * 1024 * 1024;	/* 64M */

SECTIONS
{
	. = bootstone_vma;
	.bootstone :
	{
		*(.bootstone.text);
		*(.bootstone.data);
	}

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

	free_mem = ALIGN(1024);
}
