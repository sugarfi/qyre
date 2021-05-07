ENTRY(kmain)

SECTIONS
{
    . = 0xffffffffffe02000;

    .text :
    {
        *(.text)
    }

    .rodata : 
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}
