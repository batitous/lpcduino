/* 
Linker script for LPCDuino Micro firmware (with bootloader)

The flash is organized as follow:
0x00000000 Bootloader
0x00003000 Main firmware

*/

MEMORY
{
    FLASH (rx)    : ORIGIN = 0x00003000, LENGTH = 256K - 12K 
    RAM (rwx)     : ORIGIN = 0x02000000, LENGTH = 16K
    RAM1 (rwx)    : ORIGIN = 0x02004000, LENGTH = 16K
    RAM2 (rwx)    : ORIGIN = 0x02008000, LENGTH = 4K
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        KEEP(*(.after_vectors))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data : 
    {
        __stack_start__ = .; /* start of the stack section */
        KEEP(*(.stack))
         . = ALIGN(4);
        __stack_end__ = .;   /* end of the stack section */

        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

}