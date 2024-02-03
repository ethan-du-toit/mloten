MEMORY {
  special (RWX): ORIGIN = 0x0, LENGTH = 0x1000
  cart (RWX)   : ORIGIN = 0x1000, LENGTH = 4092K    
  ram (RWX)    : ORIGIN = 0x400000, LENGTH = 256K
  mmapped (R)  : ORIGIN = 0x440000, LENGTH = 1280K
}
ENTRY( _start )
SECTIONS
{
  .special 0x0 : {
    BYTE(0x52)
    BYTE(0x56)
    BYTE(0x00)
    BYTE(0x00)
    FILL(0x73002030)
    . = 0xff0;
    BYTE(0x0)
  }  > special
  .text  : {
    *(.start)
    *(.interrupts)
    *(.text .text.*)
  } > cart
  .data : { *(.data) } > cart
  .bss : { *(.bss) } > cart
  PROVIDE(ram_begin = 0x400000);
  PROVIDE(ram_end = 0x43ffff);
  PROVIDE(stack_begin = 0x440000);
  PROVIDE(stack_end = 0x4407ff);
  PROVIDE(sys_begin = 0x440000);
  PROVIDE(sys_end = 0x44efff);
  PROVIDE(csr_begin = 0x440800);
  PROVIDE(csr_end = 0x4447ff);
  PROVIDE(input_begin = 0x444800);
  PROVIDE(input_end = 0x444bff);
  PROVIDE(screen_begin = 0x44f000);
  PROVIDE(screen_end = 0x466fff);
  PROVIDE(palette_begin = 0x461c00);
  PROVIDE(palette_end = 0x461eff);
  PROVIDE(pixels_begin = 0x44f000);
  PROVIDE(pixels_end = 0x461bff);
  PROVIDE(graph_begin = 0x467000);
  PROVIDE(graph_end = 0x47ffff);
  PROVIDE(sheet_begin = 0x467000);
  PROVIDE(sheet_end = 0x476fff);
  PROVIDE(bg_begin = 0x477000);
  PROVIDE(layer1_begin = 0x477000);
  PROVIDE(layer1_end = 0x4774ff);
  PROVIDE(layer2_begin = 0x477500);
  PROVIDE(layer2_end = 0x4779ff);
  PROVIDE(layer3_begin = 0x477a00);
  PROVIDE(layer3_end = 0x477eff);
  PROVIDE(layer4_begin = 0x477f00);
  PROVIDE(layer4_end = 0x4783ff);
  PROVIDE(bg_end = 0x4783ff);
  PROVIDE(sprite_begin = 0x479000);
  PROVIDE(sprite_end = 0x4797ff);
  PROVIDE(audio_begin = 0x480000);
  PROVIDE(audio_end = 0x4fffff);
  PROVIDE(interrupt_begin = 0x100);
  PROVIDE(interrupt_end = 0x1fc);
}

