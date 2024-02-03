#ifndef CPU_H
#define CPU_H
#include "utility.h"
#include <stdbool.h>

inline void cpu_tick(u32* restrict pc, u8* restrict ram, u32* restrict rf);

#endif
