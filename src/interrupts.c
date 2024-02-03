#include "interrupts.h"
#include "utility.h"
/* If I decide to go threaded, */
/* this needs a mutex.         */
i32 queue_interrupt(u32 mcause, bool exception, u32 pending, u8* ram) {
  i32 mstatus = l(0x441400);
  /* LW(0x441400, mstatus); */
  /* MUTEX */
  if (!(mstatus & 0x8)) {
    return -1; /* Could not schedule interrupt */
  }
  mstatus &= ~0x8; /* set mie to 0 */
  mstatus |= 0x80; /* set mpie to mie */  
  s(mstatus, 0x441400);  
  /* END MUTEX */
  s(mcause, 0x441508);
  if(!exception) {
    s(pending, 0x441510);
  } else {
    s(pending, 0x440fc0);
  }
  return 0;
}
