#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlc::sa
{

enum BuiltinType
{
  BUILTIN_TYPE_UNK,

  BUILTIN_TYPE_VOID,

  BUILTIN_TYPE_UCHAR,
  BUILTIN_TYPE_USHORT,
  BUILTIN_TYPE_UINT,
  BUILTIN_TYPE_ULONG,

  BUILTIN_TYPE_CHAR,
  BUILTIN_TYPE_SHORT,
  BUILTIN_TYPE_INT,
  BUILTIN_TYPE_LONG,

  BUILTIN_TYPE_FLOAT,
  BUILTIN_TYPE_DOUBLE,

  BUILTIN_TYPE_BOOL,

  BUILTIN_TYPE_UNION,
  BUILTIN_TYPE_STRUCT,
  BUILTIN_TYPE_ENUM,

  BUILTIN_TYPE_FUNC,
};

enum ModifierFlags
{
  MODIFIER_CONST = 1 << 0,
  MODIFIER_STATIC = 1 << 1,
  MODIFIER_INLINE = 1 << 2,
};

struct Enum
{
  std::vector<std::string> elements{};
};

struct Method;

struct Type
{
  // Used for structs, unions and enums
  std::string type_name{};

  // Used for function pointers.
  std::vector<Type> argument_types{};
  std::shared_ptr<Type> return_type{};

  // NOTE: for `type = BUILTIN_TYPE_FUNC` `pointer_count` should always be
  // greater than zero.
  int pointer_count{};

  BuiltinType type{};
};

struct Variable
{
  Type type{};
  ModifierFlags flags{};
};

struct Func
{
  Type return_type{};
  std::vector<Type> arguments{};
  std::vector<size_t> predefined_argument_positions{};
  ModifierFlags flags{};
};

struct Union;

struct Struct
{
  std::unordered_map<std::string, Variable> fields{};
  std::unordered_map<std::string, Func> methods{};
  std::unordered_map<std::string, Struct> inner_structs{};
  std::unordered_map<std::string, Enum> inner_enums{};
  std::unordered_map<std::string, Union> inner_unions{};
};

struct Union
{
  std::unordered_map<std::string, Variable> fields{};
  std::unordered_map<std::string, Func> methods{};
  std::unordered_map<std::string, Struct> inner_structs{};
  std::unordered_map<std::string, Union> inner_unions{};
};

}
