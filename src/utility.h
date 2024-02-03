#ifndef UTILITY_H
#define UTILITY_H
#pragma ONCE
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef __GNUC__
#ifndef __clang__
#define __builtin_unreachable() /* Portability */
#endif
#endif

#define SW(x, y) {ram[ (y) + 3 ] = ((u8) ((((u32) (x)) & 0xff000000) >> (u32) 24)); \
    ram[ (y) + 2] = ((u8) (( ((u32) (x))  & 0xff0000) >> (u32) 16)); \
    ram[ (y) + 1] = ((u8) (( ((u32) (x))  & 0xff00) >> (u32) 8)); \
    ram[ (y) ] = ((u8) ( ((u32) (x))  & 0xff)); }

#define LW(x, y) { (y) = (u32) (((u32) ram[ (x) +3]) << (u32) 24) +	\
      (u32) (((u32) ram[ (x) +2])  << (u32) 16) +			\
      (u32) (((u32) ram[ (x) +1])  << (u32) 8) +			\
      (u32) (((u32) ram[ (x) ])); }

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

inline u32 lw(int x, u8* ram) {
#if (!defined(__BYTE_ORDER__))||(_BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  return (u32) (((u32) ram[ (x) +3]) << (u32) 24) +   
      (u32) (((u32) ram[ (x) +2])  << (u32) 16) +    
      (u32) (((u32) ram[ (x) +1])  << (u32) 8) +     
    (u32) (((u32) ram[ (x) ]));
#else
  u32 i;		       
  memcpy(&i, (u8*) ram + x, sizeof(u32));
  return i;
#endif
}

inline void sw(u32 x, int addr, u8* ram) {
#if (!defined(__BYTE_ORDER__))||(_BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  ram[ (addr) + 3 ] = ((u8) ((((u32) (x)) & 0xff000000) >> (u32) 24));  
  ram[ (addr) + 2] = ((u8) (( ((u32) (x))  & 0xff0000) >> (u32) 16));   
  ram[ (addr) + 1] = ((u8) (( ((u32) (x))  & 0xff00) >> (u32) 8));	     
  ram[ (addr) ] = ((u8) ( ((u32) (x))  & 0xff));
#else
  memcpy(ram+addr, &x, sizeof(u32));
#endif  
}

inline u16 _lh(int x, u8* ram) {
#if (!defined(__BYTE_ORDER__))||(_BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  return    
      (u16) (((u16) ram[ (x) +1])  <<  8) +     
    (u16) (((u16) ram[ (x) ]));
#else
  u16 i;		       
  memcpy(&i, (u8*) ram + x, sizeof(u16));
  return i;
#endif
}

inline void _sh(u16 x, int addr, u8* ram) {
#if (!defined(__BYTE_ORDER__))||(_BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)  
  ram[ (addr) + 1] = ((u8) (x  & 0xff00) >> (u16) 8);	     
  ram[ (addr) ] = ((u8) (x) & 0xff);
#else
  u16 y = x & 0xffff;
  memcpy(ram+addr, &y, sizeof(u16));
#endif
}


#define l(y) lw(y, ram)
/* #define l(x) (u32) (((u32) ram[ (x) +3]) << (u32) 24) +	\
      (u32) (((u32) ram[ (x) +2])  << (u32) 16) +	\
      (u32) (((u32) ram[ (x) +1])  << (u32) 8) +	\
      (u32) (((u32) ram[ (x) ])) */
#define lh(y) _lh(y, ram)

#define s(x, y) sw(x, y, ram) 
  /* #define s(x, y) {ram[ (y) + 3 ] = ((u8) ((((u32) (x)) & 0xff000000) >> (u32) 24)); \
    ram[ (y) + 2] = ((u8) (( ((u32) (x))  & 0xff0000) >> (u32) 16)); \
    ram[ (y) + 1] = ((u8) (( ((u32) (x))  & 0xff00) >> (u32) 8)); \
    ram[ (y) ] = ((u8) ( ((u32) (x))  & 0xff)); } */
#define sh(x, y) _sh(x, y, ram)

#define unlikely(x) __builtin_expect((x), 0)
#define err_on_null(x) do { if(unlikely((x) == NULL)) {fprintf(stderr, "NULL POINTER at line %d in file %s... ABORTING", __LINE__, __FILE__); fflush(stderr); abort();}} while (0)

#define err_on_nonzero(x) do { if(unlikely((x) != 0)) {abort();}} while (0)

/* #define TEST */
/* #define DEBUG */
/* #define NOSLEEP */
/* #define NODISPLAY */
/* #define NOPPU */
/* #define PERF */
/* #define ASSERT_ASSUMP */
/* #define PJ */
#ifdef ASSERT_ASSUMP
#include <assert.h>
#define __builtin_unreachable() assert(0==1)
#endif

#define assume(x) do {if(!(x)) { __builtin_unreachable(); }} while (0)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SHEET_WIDTH 256
#define SHEET_HEIGHT 256
#ifdef DEBUG
#define printf(...) printf(__VA_ARGS__); fflush(stdout)
#endif
#endif
