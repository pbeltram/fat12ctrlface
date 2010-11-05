/*
 * $Id$
 */

MEMORY 
{
    flash    (rx) : ORIGIN = 0x00000000, LENGTH = 500K
    ram      (rw) : ORIGIN = 0x40000000, LENGTH = 32K - 32
   usbram    (rw) : ORIGIN = 0x7FD00000, LENGTH = 8K
}

__stack_end__ = 0x40000000 + 32K - 36;

SECTIONS 
{

    . = 0;
    startup : { *(.startup)} >flash

    prog : 
    {
        *(.text)
        *(.rodata)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        KEEP(*(.eh_frame))        
    } >flash

    __end_of_text__ = .;

    .data : 
    {
        __data_beg__ = .;
        __data_beg_src__ = __end_of_text__;
        *(.data)
        __data_end__ = .;
    } >ram AT>flash

    .bss : 
    {
        __bss_beg__ = .;
        *(.bss)
    } >ram

    /* Align here to ensure that the .bss section occupies space up to
    _end.  Align after .bss to ensure correct alignment even if the
    .bss section disappears because there are no input sections.  */
    . = ALIGN(32 / 8);
    _bss_end__ = . ; __bss_end__ = . ; __end__ = . ;

    . = ALIGN(32 / 8);
    _end = .;
    
    PROVIDE (end = .);

  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
}
