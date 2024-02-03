#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include "utility.h"
i32 queue_interrupt(u32 mcause, bool exception, u32 pending, u8* ram);
#endif
