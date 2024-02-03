#include "utility.h"
#include <stdio.h>
void ppu(u8* restrict /* can't hurt if i add params later */ ram, int scanline) {
  assume(scanline < 400);
  assume(scanline >= 0);
  u8* sheet = ram + 0x467000;
  u8* pixels = ram + 0x44f000;
  int bgHeight = scanline / 16;
  u8* bg = ram + 0x477000;
  u8* cl = bg + 0x500; /* Cur layer */
  for(int i = 0; i < 20; i++) {
    int layers = 4;
    u8 flags = cl[bgHeight * 20 * 4 + i * 4 + 3];
    while (layers-- && !(flags & 1)) { /* x, y, offs, FLAGS. Last flag is the transparency flag */
      cl += 0x500; /* Go to next BG layer */
      flags = cl[bgHeight * 20 * 4 + i * 4 + 3];      
    }
    if (layers <= 0) {
      continue;
    } 
    u8 x = cl[bgHeight * 20 * 4 + i * 4];
    u8 y = cl[bgHeight * 20 * 4 + i * 4 + 1];
    u8 offs = cl[bgHeight * 20 * 4 + i * 4 + 2];
    #pragma GCC unroll 16 
    for(int j = 0; j < 16; j++) {
      u8 nx = x, ny = y, noffs = offs, nflags = flags;
      u8 colour = sheet[(ny + (scanline % 16)) * SHEET_WIDTH + nx + j];
      int l = layers;
      u8* c = cl;
      while (l-- && (colour == 0 || !(nflags & 1))) {
	c += 0x500;
	nx = c[bgHeight * 20 * 4 + i * 4];
	ny = c[bgHeight * 20 * 4 + i * 4 + 1];
	noffs = c[bgHeight * 20 * 4 + i * 4 + 2];
	nflags = c[bgHeight * 20 * 4 + i * 4 + 3];
	colour = sheet[(ny + (scanline % 16)) * SHEET_WIDTH + nx + j];
      }
      pixels[SCREEN_WIDTH * scanline + i * 16 + j] = colour + noffs;
    }
    cl = bg;
  }
  /* --- */
  u8* sprites = ram + 0x479000;
  for(int i = 0; i < 256; i++) {
    u8 flags = sprites[i*8+6];
    if (!(flags & 0x1)) {
      continue;
    }
    u8 screen_y = sprites[i*8+4];
    u8 sizes = sprites[i*8+5];
    int y_size = (sizes & 0x0f) << 3;
    /* */
    u8 x = sprites[i*8];
    u8 y = sprites[i*8+1];
    u8 offs = sprites[i*8+2];
    u16 screen_x = (u16) sprites[i*8+3];
    int x_size = (sizes & 0xf0) >> 1;
    screen_x |= (flags&0x80) << 1;
    /* */
    if(!(scanline >= screen_y && scanline < (screen_y + y_size))) {
      continue;
    }

    int sprite_line = scanline - screen_y;
    if(x_size + screen_x >= 320) {
      x_size = 320 - screen_x;
    }
    #pragma GCC unroll 8
    assume((x_size % 8) == 0);
    for(int j = 0; j < x_size; j++) {
      u8 colour =  sheet[(y + sprite_line) * SHEET_WIDTH + x + j];
      if (colour != 0) { 
	pixels[SCREEN_WIDTH * scanline + screen_x + j] = colour + offs;
      }
    }
  }
  /* Fully overdrawn: Layer One */
  for(int i = 0; i < 20; i++) {
    int layers = 4;
    u8 flags = bg[bgHeight * 20 * 4 + i * 4 + 3];
    if(!(flags & 1)) { /* x, y, offs, FLAGS. Last flag is the transparency flag */
      continue;
    }

    u8 x = bg[bgHeight * 20 * 4 + i * 4];
    u8 y = bg[bgHeight * 20 * 4 + i * 4 + 1];
    u8 offs = bg[bgHeight * 20 * 4 + i * 4 + 2];
    #pragma GCC unroll 16 
    for(int j = 0; j < 16; j++) {
      u8 colour = sheet[(y + (scanline % 16)) * SHEET_WIDTH + x + j];
      if (colour != 0) {
	pixels[SCREEN_WIDTH * scanline + i * 16 + j] = colour + offs;
      }
    }
  }
}
