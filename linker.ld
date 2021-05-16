ENTRY(kmain)

SECTIONS
{
	. = 0xffffffff80000000 + 2M;

    .stivale2hdr : {
        KEEP(*(.stivale2hdr))
    }
 
    .text : {
		*(.text*)
	}
 
    .rodata : {
		*(.rodata*)
	}
 
    .data : {
		*(.data*)
	}
 
    .bss : {
		*(COMMON)
		*(.bss*)
	}
}
