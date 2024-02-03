#include "256head.h"

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

int score = 0;
int bullet = 0;
int jump = 0;
int yvel = 0;
__attribute__((interrupt)) __attribute__((used)) void vblank(void) {
  unsigned short spr_x =  sprites[3];
  spr_x |= (sprites[6] & 0x80) << 1;
  if(input[1]) {
    spr_x ++;
  } else if (input[3]) {
    spr_x --;
  }

  if (sprites[4] >= (240 - 32)) { // collide
    sprites[4] = 240-32;
    if (yvel > 0) yvel = 0;
      // UP
    if(input[0]) {
      yvel -= 15;
    }
  } else {
    yvel++;
  }
  sprites[4] += yvel;
  sprites[3] = (unsigned char) (spr_x & 0xff);
  sprites[6] &= ~0x80;
  sprites[6] |= (spr_x & 0x100) >> 1;




  //SHOOT BULLET
  if(input[6]) {
    bullet = 1;
    sprites[8 + 3] = sprites[3] + 4; /*SCR X */
    sprites[8 + 4] = sprites[4] + 4; /* SCR Y */
    sprites[8 + 6] = 1;
    sprites[8 + 6] &= ~0x80;
    sprites[8 + 6] |= (sprites[6] & 0x80) << 1;
    score++;
  }

  // MOVE BULLET
  if(bullet) {
    spr_x =  sprites[8 + 3];
    spr_x |= (sprites[8 + 6] & 0x80) << 1;
    spr_x += 2;
    if(spr_x > 310) {
      bullet = 0;
      sprites[8 + 6] = 0;
    }
    sprites[8 + 3] = (unsigned char) (spr_x & 0xff);
    sprites[8 + 6] &= ~0x80;
    sprites[8 + 6] |= (spr_x & 0x100) >> 1;

  }

  // SCORE
  if (score == 0) {
    bg[3 * 4] = 0 * 16 + 160;
    bg[3 * 4 + 1] = 16;
    bg[3 * 4 + 3] = 1;
  } else {
    int s = score;
    int z = 0;
    int digs = 0;
    /* Reverse digits */
    while(s) {
      digs++;
      z *= 10;
      z += s % 10;
      s /= 10;
    }
    for(int i = 3; z || i-3 < digs; i++) {
      int dig = z?z % 10:0;
      z /= 10;
      bg[i * 4] = (dig%5) * 16 + 160;
      bg[i * 4 + 1] = dig>=5 ? 32 : 16;
      bg[i * 4 + 3] = 1;
    }
  }
}

int main() {
  register_interrupt(&vblank, 16);
  for(int i = 0; i < 256; i++) {
    palette[i*3] =   header_data_cmap[i][0];
    palette[i*3+1] = header_data_cmap[i][1];
    palette[i*3+2] = header_data_cmap[i][2];
  }

  for(int i = 0; i < 3; i++) {
    bg[i * 4] = i * 16 + 160;
    bg[i * 4 + 1] = 0;
    bg[i * 4 + 3] = 1;
  }
  volatile char* l2 = bg + 0x500;
  for(int i = 0; i < 320 / 16; i++) {
    for (int j = 0; j < 240 / 16; j++) {
      l2 [ ((j * 320 / 16) + i) * 4     ] = (signed char) 70 + ((i * 16) >= 166? i * 16 - 166 : i * 16) ;
      l2 [ ((j * 320 / 16) + i) * 4 + 1 ] = (signed char) 90 + ((j * 16) >= 146? j * 16 - 146 : j * 16) ;
      l2 [ ((j * 320 / 16) + i) * 4 + 3 ] = 1;
    }
  }
  for(int i = 0; i < 256 * 256; i++) {
    sheet[i] = header_data[i];
  }
  sprites[0] = 30; /* X */
  sprites[1] = 30; /* Y */
  sprites[2] = 0; /* OFFS */
  sprites[3] = 100; /*SCR X */
  sprites[4] = 50; /* SCR Y */
  sprites[5] = 0x24; /* SIZES */
  sprites[6] = 1;

  sprites[8 + 0] = 0; /* X */
  sprites[8 + 1] = 248; /* Y */
  sprites[8 + 2] = 0; /* OFFS */
  sprites[8 + 5] = 0x11; /* SIZES */
  sprites[8 + 6] = 0x0; /* SIZES */

  while(1) {
    asm volatile ( "wfi" );
  }
}


