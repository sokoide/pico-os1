ENTRY(reset_handler)

MEMORY {
    ROM (rx)	: ORIGIN = 0x10000000, LENGTH = 1024K
    FS (xrw)    : ORIGIN = 0x10100000, LENGTH = 1024K
    RAM (xrw)	: ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS {
    boot2 : {
        __boot2_start__ = .;
        KEEP (*(.boot2))
        __boot2_end__ = .;
    } > ROM
    ASSERT(__boot2_end__ - __boot2_start__ == 256,
        "ERROR: Pico second stage bootloader must be 256 bytes in size")

    .text : {
        __vector_org = .;
        KEEP (*(.vector))

        *(EXCLUDE_FILE(*libgcc.a: *libc.a:*lib_a-mem*.o *libm.a:) .text*)

        __start = .;
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    } >ROM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ROM

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > ROM
    __exidx_end = .;

	.fs_section : {
		__fs_start = .;
		KEEP(*(.fs_section))
		koidos.o (.fs_section)
		__fs_end = .;
	} > FS

    __data_org = .;
    .data : AT(__data_org) {
        __data_start = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        __data_end = .;
    } >RAM

    .bss ALIGN(4) (NOLOAD) : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
        __end = .;
    } >RAM

    ASSERT(_binary_koidos_fat_end - _binary_koidos_fat_start == 720*1024,
        "ERROR: koidos must be 720MB")

}
