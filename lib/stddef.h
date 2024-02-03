#ifndef __STDDEF_H
#define __STDDEF_H
#define NULL ((void*)0)
typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef short wchar_t;
#define offsetof(t, m) __builtin_offsetof(t, m)
#endif
