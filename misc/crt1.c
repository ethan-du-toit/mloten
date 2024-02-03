extern int main();
/* riscv64-linux-gnu-gcc -O3 -Tmloten.ld -Wl,--build-id=none -nostdlib -nostartfiles -march=rv32im -mabi=ilp32 ./crt1.c -c -o crt1.o */
__attribute__((section(".start"))) void _start() {
  main();
}
/* You may think this is just
 * wasting 4 bytes of memory.
 * Think again. Look back to
 * the spec... HOW MANY bytes
 * does a standard (PLIC) 
 * vectored interrupt get to
 * jump to its interrupt?
 * One. A single byte. This
 * means we have to use PC-
 *-relative addressing - aka
 * we can only be within 1M
 * of the interrupt handler.
 * It is therefore of utmost
 * importance that we put 
 * the interrupt handlers at
 * locations as close to the 
 * beginning of linkage as
 * possible. Hence the hacky
 * special ".interrupt" 
 * section. In any case -
 * since this is hardcoded to
 * go to 0x1000 directly on
 * "boot"... this is useful
 * to jump past the interrupt
 * handlers! (And is necessary
 * to put them reasonably
 * close at all.)              */
