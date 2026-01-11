#ifndef __SOLC_DEFS_H__
#define __SOLC_DEFS_H__

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef float f32;
typedef double f64;

#ifdef __cplusplus__
#define CPP_GUARD_TOP() extern "C" {
#define CPP_GUARD_BOTTOM() }
#else
#define CPP_GUARD_TOP()
#define CPP_GUARD_BOTTOM()
#endif

#ifdef __cplusplus__
typedef bool b8;
#else
typedef _Bool b8;
#define true 1
#define false 0
#endif
typedef int b32;

typedef size_t sz;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define nullptr (void *)0

#endif // __SOLC_DEFS_H__
