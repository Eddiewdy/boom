OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  .text : {
    *(.text.init)
    *(.text)
  }
  .rodata : { *(.rodata) }
  .data : {
    *(.data)
    . = ALIGN(8);
    tohost = .;
    QUAD(0);
    fromhost = .;
    QUAD(0);
  }
  .bss : {
    _bss_start = .;
    *(.bss)
    _bss_end = .;
  }
  . = ALIGN(16);
  . += 0x1000;
  _stack_top = .;
}