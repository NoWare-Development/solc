#include "hash.h"

hash_t hash_function_fnv_1a_cstr(const void *key)
{
#if IS_64BIT == 1
#define FNV_PRIME 0x00000100000001b3ULL
#define FNV_OFFSET_BASIS 0xcbf29ce484222325ULL
#else
#define FNV_PRIME 0x01000193U
#define FNV_OFFSET_BASIS 0x811c9dc5U
#endif
  hash_t hash = FNV_OFFSET_BASIS;
  const char *s = key;
  for (; *s; s++) {
    hash ^= *s;
    hash *= FNV_PRIME;
  }
  return hash;
}

hash_t hash_function_i8(const void *key)
{
  u32 key_32 = *(u8 *)key;
  return hash_function_i32(&key_32);
}

hash_t hash_function_i16(const void *key)
{
  u32 key_32 = *(u16 *)key;
  return hash_function_i32(&key_32);
}

hash_t hash_function_i32(const void *key)
{
#define HASH_I32_MUL 0x45d9f3bU
  u32 key_32 = *(u32 *)key;
  key_32 = ((key_32 >> 16) ^ key_32) * HASH_I32_MUL;
  key_32 = ((key_32 >> 16) ^ key_32) * HASH_I32_MUL;
  key_32 = (key_32 >> 16) ^ key_32;
  return key_32;
}

hash_t hash_function_i64(const void *key)
{
#define HASH_I64_MUL_1 0xbf58476d1ce4e5b9ULL
#define HASH_I64_MUL_2 0x94d049bb133111ebULL
  u64 key_64 = *(u64 *)key;
  key_64 = (key_64 ^ (key_64 >> 30)) * HASH_I64_MUL_1;
  key_64 = (key_64 ^ (key_64 >> 27)) * HASH_I64_MUL_2;
  key_64 ^= key_64 >> 31;
  return key_64;
}

hash_t hash_function_UNDEFINED(const void *x)
{
  SOLC_NOREACH();
}
