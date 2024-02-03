#ifndef ML_STD_H
#define ML_STD_H
#define true 1
#define false 0
typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long int u64;
typedef signed long long int i64;
typedef _Bool bool;

extern int sheet_begin;
volatile char* sheet = (char*) &sheet_begin;

extern int bg_begin;
volatile char* bg = (char*) &bg_begin;

extern int interrupt_begin;
volatile void* ints = &interrupt_begin;

extern int csr_begin;
volatile int* csrs  = &csr_begin;

extern int palette_begin;
volatile char* palette = (char*) &palette_begin;

extern int sprite_begin;
volatile unsigned char* sprites = (unsigned char*) &sprite_begin;

extern int input_begin;
volatile char* input = (char*) &input_begin;


void register_interrupt(void (*fn)(), int posn) {
  asm volatile (
		"csrwi mie, 0"
		);  
  volatile int* timer_interrupt = ((int*) ints) + posn;
  int rel_addr = ((void*) fn) - (void*) timer_interrupt;
  *timer_interrupt =
    ((rel_addr & 0x7fc) << 20)   |
    ((rel_addr & 0x800) << 9)    |
    (rel_addr  & 0xff000)        |
    0x6f;
  asm volatile (
		"csrwi mie, 1"
		);
}

u8 nsprites;
typedef struct sprite {
  bool visiblep;
  u8 sheetx;
  u8 sheety;
  u16 x;
  u8 y;
  u8 sizes;
  i8 xvel;
  i8 yvel;
  i8 xadd;
  i8 yadd;
  bool groundedp;
  u8 anistage;
} sprite;

sprite our_sprites[105];

bool postvblank;
bool indicate;

typedef u8 level[20][15];

u8 current_level = -1;
level levels[16];

void display_sprites() {
  for(int i = 0; i < nsprites; i++) {
    sprite it = our_sprites[i];
    sprites[i*8+0] = it.sheetx;
    sprites[i*8+1] = it.sheety;
    sprites[i*8+3] = it.x & 0xffff;
    sprites[i*8+4] = it.y;
    sprites[i*8+5] = it.sizes;
    sprites[i*8+6] = it.visiblep | ((it.x >> 1) & 0x80);
  }
}

void die() {
  char* x = -1;
  *x = 1;
}
#endif
