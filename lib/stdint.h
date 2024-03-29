#ifndef __STDINT_H
#define __STDINT_H

typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef int int32_t;
typedef short int16_t;
typedef signed char int8_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define INT64_MAX (9223372036854775807)
#define INT64_MIN (-9223372036854775808)

#define UINT64_MAX (18446744073709551615)

#define INT32_MAX (2147483647)
#define INT32_MIN (-2147483648)

#define INT16_MAX (32767)
#define INT16_MIN (-32768)

#define INT8_MAX (127)
#define INT8_MIN (-128)

#define UINT32_MAX 4294967295
#define UINT16_MAX 65535
#define UINT8_MAX 255

typedef int intptr_t;
typedef unsigned int uintptr_t;

typedef long long intmax_t;
typedef unsigned long long uintmax_t;

#endif
