#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#ifndef NODISPLAY
#include <SDL2/SDL.h>
#include <signal.h>
#include "display.h"
#endif
#include "cpu.h"
#include "utility.h"
#ifndef NOPPU
#include "ppu.h"
#endif
#include "interrupts.h"
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif


void* stored __attribute__((unused));
static inline void ml(FILE* restrict f)  {
  _Static_assert((((-1) >> 1) == -1), "Your compiler does not use arithmetic left shift on signed ints. Please stop.");
  u8* ram = malloc(sizeof(u8) * 5 * 1024 * 1024);
  err_on_null(ram);
#ifndef NODISPLAY  
  SDL_Window* window = SDL_CreateWindow("MLOTEN2k",
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					320,
					240,
					0);
  err_on_null(window);
  SDL_Renderer* renderer = SDL_CreateRenderer(window,
					      -1,
					      0);
  err_on_null(renderer);
  SDL_Surface* pix_surface = SDL_CreateRGBSurfaceFrom(ram + 0x44f000,
						      320,
						      240,
						      8,
						      sizeof(u8) * 320,
						      0,
						      0,
						      0,
						      0);
  err_on_null(pix_surface);
#endif
  u32 rf[32] = {0};
  /* 0x000000 - 0x3fffff CART MEMORY 
   *     0x0000 - 0x0FFF special memory (interrupt vector, etc) 
   *     0x1000 - 0x3FFFFF cart space
   * 0x400000 - 0x43ffff General-purpose RAM
   * 0x440000 - 0x44efff System control RAM (including the stack)
   *     0x440000 - 0x4407ff Stack space
   *     0x440800 - 0x4447ff CSR space
   * 0x44f000 - 0x466fff Screen RAM
   * 0x467000 - 0x47ffff Graphics RAM
   * 0x480000 - 0x4fffff Audio RAM */
  rf[2] = 0x4407ff; /* Set up stack pointer */  
  u32 pc = 0x1000 - 4;
  fread(ram, 1, 4194304, f);
  fclose(f);
  memset(ram + 0x440000, 0, 0x44efff-0x440000);
  memset(ram + 0x467000, 0, 0x47ffff-0x467000);  
#ifndef NODISPLAY  
  Uint8 r[8] = { 0, 36, 73, 109, 146, 182, 219, 255 };
  Uint8 g[8] = { 0, 36, 73, 109, 146, 182, 219, 255 };
  Uint8 b[4] = { 0, 85, 170, 255 };
  int curColor = 0;
  for(int i = 0; i < 8; ++i ) {
    for(int j = 0; j < 8; ++j ) {
      for(int k = 0; k < 4; ++k ) {
	  ram[0x461c00 + curColor*3] = r[i];
	  ram[0x461c00 + curColor*3+1] = g[j];
	  ram[0x461c00 + curColor*3+2] = b[k];
	  curColor++;
	}
    }
  }
  for (int i = 0; i < 320 * 240; i++) {
    ram[0x44f000 + i] = i % 256;
  }

#endif  
  /* set MIE to 1 */
  /* set MPP to 3 (machine mode) */
  s(0x1808, 0x441400);
  s(0x102, 0x441414); /* interrupts vectored to 0x100 */
  /* set all interrupts enabled */
  s(0x888, 0x441410);
  /* mtime and mtimecmp are within a contiguous 16-byte zone 0xBC0 -> ...*/
  s(-1, 0x443708); /* Make sure mtimecmp */
  s(-1, 0x44370c); /* aint go off... */
  /* lhlh */
  /* The architecture is little-endian */
  
#ifndef NODISPLAY  
  int time_to_disp = 0;
#endif
  
#ifndef NOPPU  
  int scanline = 0;
  int time_to_ppu = 0;
#endif
#ifdef PERF  
  int z = 0;
  int n = 0;
#endif

#ifdef _WIN32
  //placeholder
  FILETIME t_x;
  u64 t_to;
  GetSystemTimePreciseAsFileTime(&t_x);
  t_to = t_x.dwLowDateTime + (((u64) t_x.dwHighDateTime) << 32); 
  t_to += 16666666;
#else
  struct timespec t_to;
  clock_gettime(CLOCK_MONOTONIC, &t_to);
  t_to.tv_nsec += 16666666;
  if(t_to.tv_nsec > 1000000000) {
    t_to.tv_sec++;
    t_to.tv_nsec -= 1000000000;
  }
#endif
  while (1) {
#ifndef NODISPLAY    
    time_to_disp++;
#endif

#ifndef NOPPU    
    time_to_ppu++;
#endif    
    
#ifdef PERF    
    n++;
#endif
    
    pc += 4;
    cpu_tick(&pc, ram, rf);
    
#ifndef NOPPU    
    if (scanline < 240 && time_to_ppu == 364) {
      ppu(ram, scanline);
      scanline++;      
      if (scanline == 240) {
	scanline = 0;	
	/* VBLANK */
	SDL_PumpEvents();
	const u8* state = SDL_GetKeyboardState(NULL);
#define Key(keyboard_key, ram_location) do {	\
	  if (state[ (keyboard_key) ]) {	\
	    ram[ 0x444800 + ram_location ] = 1; \
	  } else {				\
	    ram[ 0x444800 + ram_location ] = 0; \
	  }} while (0)      
	Key(SDL_SCANCODE_UP, 0);
	Key(SDL_SCANCODE_RIGHT, 1);
	Key(SDL_SCANCODE_DOWN, 2);
	Key(SDL_SCANCODE_LEFT, 3);
	Key(SDL_SCANCODE_A, 4);
	Key(SDL_SCANCODE_S, 5);
	Key(SDL_SCANCODE_Z, 6);
	Key(SDL_SCANCODE_X, 7);
	Key(SDL_SCANCODE_Q, 8);
	Key(SDL_SCANCODE_W, 9);
	Key(SDL_SCANCODE_COMMA, 10);
	Key(SDL_SCANCODE_PERIOD, 11);
	queue_interrupt(16, 0, 0x800, ram);

      }
      time_to_ppu = 0;
    }
#endif
    
#ifndef NODISPLAY          
    if (time_to_disp == 174763) {
      display(renderer, pix_surface, ram);
      time_to_disp = 0;
#ifdef _WIN32
      GetSystemTimePreciseAsFileTime(&t_x);
      u64 t_from = t_x.dwLowDateTime + (((u64) t_x.dwHighDateTime) << 32);
      if(t_from < t_to) {
	Sleep((t_to - t_from) / 1000000);//Jesus
      } 
      GetSystemTimePreciseAsFileTime(&t_x);
      t_to = t_x.dwLowDateTime + (((u64) t_x.dwHighDateTime) << 32); 
      t_to += 16666666;
      //printf("S . to . G\n"); fflush(stdout);
#else      
      clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t_to, NULL);
      clock_gettime(CLOCK_MONOTONIC, &t_to);
      t_to.tv_nsec += 16666666;
      if(t_to.tv_nsec > 1000000000) {
	t_to.tv_sec++;
	t_to.tv_nsec -= 1000000000;
      }
#endif      
    }
#endif
#ifdef PERF    
    if(n == 0x1000000) {
      printf("MEGA\n");
      n = 0;
      z++;
    }
    if(z==20) {
      exit(0);
    }
#endif    
  }
  free(ram);
  abort();
}

int main (int argc, char* argv[]) {
  FILE* f = fopen(argv[1], "r");
#ifndef NODISPLAY  
  SDL_Init(SDL_INIT_VIDEO); 
  signal(SIGINT, exit); 
#endif
  ml(f);
#ifdef TEST
  i32 opcode, imm, rs1, rs2, rd, funct7, funct3;
  /* Test time */
#define test(x) decode_instruction( x , &opcode, &imm, &rs1, &rs2, &rd, &funct7, &funct3);
  test( 0b10110010110101011010100100000011 );
  /*test( 0b101100101101_01011_010_10010_0000011 );*/
  assert(opcode == 0b0000011);
  assert(rs1 == 0b01011);
  assert(imm == 0b11111111111111111111101100101101);
  assert(funct3 == 0b010);
  assert(rd == 0b10010);
  printf("Finished testing I-format instructions.\n");
  test( 0b00110001010100101111100110110011);
  /*  test( 0b0011000_10101_00101_111_10011_0110011);*/
  assert(opcode == 0b0110011);
  assert(rs1 == 0b00101);
  assert(rs2 == 0b10101);
  assert(funct3 == 0b111);
  assert(rd  == 0b10011);
  assert(funct7 == 0b11000);
  printf("Finished testing R-format instructions.\n");
  test( 0b10011010011010110101011010100011 );
  /* test(0b1001101_00110_10110_101_01101_0100011) */
  assert(opcode == 0b0100011);
  assert(rs1 == 0b10110);
  assert(rs2 == 0b00110);
  assert(funct3 == 0b101);
  assert(imm == 0b11111111111111111111100110101101);
  printf("Finished testing S-format instructions.\n");
  test(0b10101100001110110001101111100011);
  /* test(0b1_010110_00011_10110_001_1011_1_1100011) */
  assert(opcode == 0b1100011);
  assert(rs2 == 0b00011);
  assert(rs1 == 0b10110);
  assert(funct3 == 1);
  assert(imm == 0b11111111111111111111101011010110);
  printf("Finished testing B-format instructions.\n");
  test( 0b10111011100110011101011000110111);
  /* test ( 0b10111011100110011101_01100_0110111 ) */
  assert(opcode == 0b0110111);
  assert(rd == 0b01100);
  assert(imm == 0b10111011100110011101000000000000);
  printf("Finished testing U-format instructions.\n");
  test(0b11011001010110010001001001101111);
  /*test(0b1_1011001010_1_10010001_00100_1101111)*/
  assert(opcode == 0b1101111);
  assert(rd == 0b00100);
  assert(imm == 0b11111111111110010001110110010100);
  printf("Finished testing J-format instructions.\n");
#endif
}
