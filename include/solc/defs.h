#ifndef __SOLC_DEFS_H__
#define __SOLC_DEFS_H__

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define LIBSOLC_VERSION 0.0.1

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef intptr_t sptr;
typedef uintptr_t uptr;

#ifdef __cplusplus__
#define __SOLC_CPP_GUARD_TOP() extern "C" {
#define __SOLC_CPP_GUARD_BOTTOM() }
#else
#define __SOLC_CPP_GUARD_TOP()
#define __SOLC_CPP_GUARD_BOTTOM()
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

#define SOLC_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SOLC_MAX(a, b) ((a) > (b) ? (a) : (b))

#define nullptr (void *)0

#define __SOLC_CONCAT_IMPL(x, y) x##y
#define __SOLC_CONCAT(x, y) __SOLC_CONCAT_IMPL(x, y)

#define __SOLC_QUOTE(name) #name
#define __SOLC_MACRO_STR(macro) __SOLC_QUOTE(macro)

#define __SOLC_UNIQUE_ID() __COUNTER__

#ifdef _MSC_VER
#include <intrin.h>
#define __solc_trap() __debugbreak()
#define __solc_noreach() __assume(0)
#define __solc_assume(expr) __assume((expr))
#elif defined(__GNUC__) || defined(__clang__)
#define __solc_trap() __builtin_trap()
#define __solc_noreach() __builtin_unreachable()
#define __solc_assume(expr) __attribute__((assume((expr))))
#else
#include <stdlib.h>
#define __solc_trap() exit(-1)
#define __solc_noreach() exit(-1)
#define __solc_assume(expr)
#endif

#define SOLC_TODO(msg)                                                       \
  {                                                                          \
    fprintf(stderr, "(%s:%i) Not implemented yet: %s\n", __FILE__, __LINE__, \
            msg);                                                            \
    __solc_trap();                                                           \
  }

#ifdef _DEBUG
#define SOLC_NOREACH() __solc_noreach()
#else
#define SOLC_NOREACH()                                                       \
  {                                                                          \
    fprintf(stderr, "(%s:%i) Reached point that should never be reached.\n", \
            __FILE__, __LINE__);                                             \
    __solc_trap();                                                           \
  }
#endif

#ifdef _DEBUG
#define SOLC_ASSUME(expr)                                             \
  if SOLC_UNLIKELY (!(expr)) {                                        \
    fprintf(stderr, "(%s:%i) Assumption \"" #expr "\" is not met.\n", \
            __FILE__, __LINE__);                                      \
    __solc_trap();                                                    \
  }
#else
#define SOLC_ASSUME(expr) __solc_assume(expr)
#endif

#ifdef _DEBUG
#define SOLC_ASSERT(expr)                                                  \
  if SOLC_UNLIKELY (!(expr)) {                                             \
    fprintf(stderr, "(%s:%i) Assertion \"" #expr "\" failed.\n", __FILE__, \
            __LINE__);                                                     \
    __solc_trap();                                                         \
  }
#else
#define SOLC_ASSERT(expr)                                                  \
  if SOLC_UNLIKELY (!(expr)) {                                             \
    fprintf(stderr, "(%s:%i) Assertion \"" #expr "\" failed.\n", __FILE__, \
            __LINE__);                                                     \
    exit(-1);                                                              \
  }
#endif

#if defined(__clang__)
#define SOLC_LIKELY(expr) ((expr)) [[clang::likely]]
#define SOLC_UNLIKELY(expr) ((expr)) [[clang::unlikely]]
#elif defined(__GNUC__)
#define SOLC_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#define SOLC_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#else
#define SOLC_LIKELY ((expr))
#define SOLC_UNLIKELY ((expr))
#endif

// SOLC_UNUSED is used for temporal debugging stuff that shouldn't end up in
// master branch.
// SOLC_UNUSED_PERMIT is used when you need magic to happen :^)
#ifdef _DEBUG
#define SOLC_UNUSED(_X) (void)(_X)
#else
#define SOLC_UNUSED(_X) SOLC_UNUSED_IS_AN_ERROR
#endif
#define SOLC_UNUSED_PERMIT(_X) (void)(_X)

#if INTPTR_MAX == INT32_MAX
#define IS_64BIT 0
#elif INTPTR_MAX == INT64_MAX
#define IS_64BIT 1
#else
#error "SOLC supports only 64 and 32-bit architectures."
#endif

#endif // __SOLC_DEFS_H__
