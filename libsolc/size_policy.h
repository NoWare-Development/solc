#ifndef __SOLC_SIZE_POLICY_H__
#define __SOLC_SIZE_POLICY_H__

// * HASHTABLE_SIZE_POLICY_FIXED stores data of an object provided
//   to the `hashtable_put()'.
// * HASHTABLE_SIZE_POLICY_VARIABLE stores a pointer to a copy of data
//   of an object provided to the `hashtable_put()' (function pointer of type
//   `hashtable_get_size_function_t' must be provided in `hashtable_create()').
typedef enum {
  SIZE_POLICY_FIXED = 0,
  SIZE_POLICY_VARIABLE = 1,
} size_policy_t;

#endif // __SOLC_SIZE_POLICY_H__
