#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <parser/ast.hpp>
#include <string>
#include <vector>

namespace solc
{

enum class BuiltinTypeGroup : uint16_t {
  NONE = 0,
  UINT,
  INT,
  FLOAT,
  COMPLEX,
};

static constexpr size_t sz_bits_1 = 1;
static constexpr size_t sz_bits_8 = 8;
static constexpr size_t sz_bits_16 = 16;
static constexpr size_t sz_bits_32 = 32;
static constexpr size_t sz_bits_64 = 64;
static constexpr size_t sz_bits_invalid = 0;

#define __BTYPE_DEF(group, size)                                        \
  (((__COUNTER__ & 0xFFFF) << 16) | (((uint16_t)(group) & 0xFF) << 8) | \
   ((size) & 0xFF))

enum class BuiltinType : uint32_t {
  UNK = __BTYPE_DEF(BuiltinTypeGroup::NONE, sz_bits_invalid),

  VOID = __BTYPE_DEF(BuiltinTypeGroup::NONE, sz_bits_invalid),

  UCHAR = __BTYPE_DEF(BuiltinTypeGroup::UINT, sz_bits_8),
  USHORT = __BTYPE_DEF(BuiltinTypeGroup::UINT, sz_bits_16),
  UINT = __BTYPE_DEF(BuiltinTypeGroup::UINT, sz_bits_32),
  ULONG = __BTYPE_DEF(BuiltinTypeGroup::UINT, sz_bits_64),

  CHAR = __BTYPE_DEF(BuiltinTypeGroup::INT, sz_bits_8),
  SHORT = __BTYPE_DEF(BuiltinTypeGroup::INT, sz_bits_16),
  INT = __BTYPE_DEF(BuiltinTypeGroup::INT, sz_bits_32),
  LONG = __BTYPE_DEF(BuiltinTypeGroup::INT, sz_bits_64),

  FLOAT = __BTYPE_DEF(BuiltinTypeGroup::FLOAT, sz_bits_32),
  DOUBLE = __BTYPE_DEF(BuiltinTypeGroup::FLOAT, sz_bits_64),

  BOOL = __BTYPE_DEF(BuiltinTypeGroup::UINT, sz_bits_1),

  STRUCT = __BTYPE_DEF(BuiltinTypeGroup::COMPLEX, sz_bits_invalid),
  ENUM = __BTYPE_DEF(BuiltinTypeGroup::INT, sz_bits_invalid),
  UNION = __BTYPE_DEF(BuiltinTypeGroup::COMPLEX, sz_bits_invalid),

  FUNC = __BTYPE_DEF(BuiltinTypeGroup::NONE, sz_bits_invalid),
};

static inline constexpr BuiltinTypeGroup get_builtin_type_group(BuiltinType type)
{
  return static_cast<BuiltinTypeGroup>(((uint32_t)type >> 8) & 0xFF);
}

static inline constexpr size_t get_builtin_type_size(BuiltinType type)
{
  return (uint32_t)type & 0xFF;
}

static inline constexpr size_t get_builtin_type_index(BuiltinType type)
{
  return ((uint32_t)type >> 16) & 0xFF;
}

struct Symbol;

struct Type final {
  struct FuncArg final {
    std::shared_ptr<Type> type;
    std::shared_ptr<AST> def_expr;

    FuncArg(std::shared_ptr<Type> type, std::shared_ptr<AST> def_expr = nullptr)
      : type(type)
      , def_expr(def_expr)
    {
    }
  };

  std::vector<size_t> array_sizes{};
  std::vector<FuncArg> arguments{};

  std::shared_ptr<Type> return_type{};

  Symbol *symbol_binding{};

  size_t pointer_indirection;

  BuiltinType builtin_type;

  Type() = default;
  Type(BuiltinType builtin_type, size_t pointer_indirection)
    : pointer_indirection(pointer_indirection)
    , builtin_type(builtin_type)
  {
  }
  Type(BuiltinType builtin_type, std::vector<size_t> &&array_sizes,
       size_t pointer_indirection)
    : array_sizes(array_sizes)
    , pointer_indirection(pointer_indirection)
    , builtin_type(builtin_type)
  {
  }
  Type(BuiltinType builtin_type, Symbol *symbol_binding,
       size_t pointer_indirection)
    : symbol_binding(symbol_binding)
    , pointer_indirection(pointer_indirection)
    , builtin_type(builtin_type)
  {
  }
  Type(BuiltinType builtin_type, Symbol *symbol_binding,
       std::vector<size_t> &&array_sizes, size_t pointer_indirection)
    : array_sizes(array_sizes)
    , symbol_binding(symbol_binding)
    , pointer_indirection(pointer_indirection)
    , builtin_type(builtin_type)
  {
  }

  Type(std::shared_ptr<Type> return_type, size_t pointer_indirection)
    : return_type(return_type)
    , pointer_indirection(pointer_indirection + 1)
    , builtin_type(BuiltinType::FUNC)
  {
  }
  Type(std::vector<FuncArg> &&arguments, size_t pointer_indirection)
    : arguments(arguments)
    , return_type(create_basic(BuiltinType::VOID))
    , pointer_indirection(pointer_indirection + 1)
    , builtin_type(BuiltinType::FUNC)
  {
  }
  Type(std::shared_ptr<Type> return_type, std::vector<FuncArg> &&arguments,
       size_t pointer_indirection)
    : arguments(arguments)
    , return_type(return_type)
    , pointer_indirection(pointer_indirection + 1)
    , builtin_type(BuiltinType::FUNC)
  {
  }
  Type(std::shared_ptr<Type> return_type, std::vector<size_t> &&array_sizes,
       size_t pointer_indirection)
    : array_sizes(array_sizes)
    , return_type(return_type)
    , pointer_indirection(pointer_indirection + 1)
    , builtin_type(BuiltinType::FUNC)
  {
  }
  Type(std::vector<FuncArg> &&arguments, std::vector<size_t> &&array_sizes,
       size_t pointer_indirection)
    : array_sizes(array_sizes)
    , arguments(arguments)
    , return_type(create_basic(BuiltinType::VOID))
    , pointer_indirection(pointer_indirection + 1)
    , builtin_type(BuiltinType::FUNC)
  {
  }
  Type(std::shared_ptr<Type> return_type, std::vector<FuncArg> &&arguments,
       std::vector<size_t> &&array_sizes, size_t pointer_indirection)
    : array_sizes(array_sizes)
    , arguments(arguments)
    , return_type(return_type)
    , pointer_indirection(pointer_indirection + 1)
    , builtin_type(BuiltinType::FUNC)
  {
  }

  std::shared_ptr<Type> copy() const;

  static std::shared_ptr<Type> create_basic(BuiltinType builtin_type,
                                            size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(builtin_type, pointer_indirection);
  }
  static std::shared_ptr<Type> create_array(BuiltinType builtin_type,
                                            std::vector<size_t> array_sizes,
                                            size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(builtin_type, std::move(array_sizes),
                                  pointer_indirection);
  }
  static std::shared_ptr<Type> create_complex(BuiltinType builtin_type,
                                              Symbol *symbol_binding,
                                              size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(builtin_type, symbol_binding,
                                  pointer_indirection);
  }
  static std::shared_ptr<Type>
  create_complex_array(BuiltinType builtin_type, Symbol *symbol_binding,
                       std::vector<size_t> array_sizes,
                       size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(builtin_type, symbol_binding,
                                  std::move(array_sizes), pointer_indirection);
  }

  static std::shared_ptr<Type>
  create_function_pointer(size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(create_basic(BuiltinType::VOID),
                                  pointer_indirection);
  }
  static std::shared_ptr<Type>
  create_function_pointer_array(std::vector<size_t> array_sizes,
                                size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(create_basic(BuiltinType::VOID),
                                  std::move(array_sizes), pointer_indirection);
  }

  static std::shared_ptr<Type>
  create_function_pointer(std::shared_ptr<Type> return_type,
                          size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(return_type, pointer_indirection);
  }
  static std::shared_ptr<Type>
  create_function_pointer_array(std::shared_ptr<Type> return_type,
                                std::vector<size_t> array_sizes,
                                size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(return_type, std::move(array_sizes),
                                  pointer_indirection);
  }
  static std::shared_ptr<Type>
  create_function_pointer(std::vector<FuncArg> arguments,
                          size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(std::move(arguments), pointer_indirection);
  }
  static std::shared_ptr<Type>
  create_function_pointer_array(std::vector<FuncArg> arguments,
                                std::vector<size_t> array_sizes,
                                size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(std::move(arguments), std::move(array_sizes),
                                  pointer_indirection);
  }
  static std::shared_ptr<Type>
  create_function_pointer(std::shared_ptr<Type> return_type,
                          std::vector<FuncArg> arguments,
                          size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(return_type, std::move(arguments),
                                  pointer_indirection);
  }
  static std::shared_ptr<Type> create_function_pointer_array(
    std::shared_ptr<Type> return_type, std::vector<FuncArg> arguments,
    std::vector<size_t> array_sizes, size_t pointer_indirection = 0)
  {
    return std::make_shared<Type>(return_type, std::move(arguments),
                                  std::move(array_sizes), pointer_indirection);
  }

  bool is_array() const;
  bool is_pointer() const;
  bool is_explicit_pointer() const;
  bool is_integer() const;
  bool is_floating() const;
  bool is_complex() const;
  bool is_enum() const;
  bool is_function_pointer() const;

  const std::vector<size_t> &get_array_sizes() const;
  size_t get_pointer_indirection() const;
  size_t get_full_pointer_indirection() const;

  Symbol *get_symbol_binding() const;

  const std::shared_ptr<Type> &get_return_type() const;
  const std::vector<FuncArg> &get_function_arguments() const;

  std::string to_string() const;
};

bool can_convert_types(const std::shared_ptr<Type> &from,
                       const std::shared_ptr<Type> &to);
std::shared_ptr<Type> get_converted_type(const std::shared_ptr<Type> &from,
                                         const std::shared_ptr<Type> &to);
BuiltinType get_converted_builtin_type(BuiltinType from, BuiltinType to);

}
