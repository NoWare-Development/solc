#pragma once

#include <parser/ast.hpp>
#include <semantic-analyzer/scope.hpp>
#include <semantic-analyzer/types.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlc::sa
{

class SemanticAnalyzer
{
public:
  SemanticAnalyzer (AST root) : _root (root) {}
  ~SemanticAnalyzer () = default;

  bool analyze ();

  struct SAError
  {
    enum ErrType
    {
      // 1
      SA_ERR_TYPE_WRONG_AST_TYPE,

      // 1
      SA_ERR_TYPE_NO_NAME_FOR_VARIABLE,

      // 1
      SA_ERR_TYPE_WRONG_NUMBER_OF_ARRAYS_CHILDREN,
      SA_ERR_TYPE_WRONG_NUMBER_OF_POINTERS_CHILDREN,
      SA_ERR_TYPE_WRONG_NUMBER_OF_INTLIT_CHILDREN,
      SA_ERR_TYPE_WRONG_NUMBER_OF_FLOATLIT_CHILDREN,
      SA_ERR_TYPE_WRONG_NUMBER_OF_EXPR_CHILDREN,

      // 1
      SA_ERR_TYPE_TYPE_HAS_NO_PLAIN_TYPE,
      SA_ERR_TYPE_TYPE_IS_NOT_DEFINED,

      // 1
      SA_ERR_TYPE_INVALID_TYPESPEC,

      // 2
      SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_EXPR,
      SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_VARDEF,

      // 1
      SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_CAST,

      // 1
      SA_ERR_TYPE_CANNOT_CONVERT_FLOAT_TO_INTTYPE_TYPESPEC,

      // 1
      SA_ERR_TYPE_UNDECLARED_VARIABLE,
      SA_ERR_TYPE_UNDECLARED_FUNCTION,

      // 1
      SA_ERR_TYPE_DEREFERENCING_NONPOINTER_VALUE,

      // 1
      SA_ERR_TYPE_BINARY_NOT_ON_NON_INTEGER,
      SA_ERR_TYPE_BINARY_NOT_ON_A_POINTER,

      // 1
      SA_ERR_TYPE_NEGATIVE_OF_VOID,
      SA_ERR_TYPE_NEGATIVE_OF_POINTER,
      SA_ERR_TYPE_NEGATIVE_OF_STRUCT,
      SA_ERR_TYPE_NEGATIVE_OF_UNION,

      // 1
      SA_ERR_TYPE_VARIABLE_ALREADY_DECLARED,
      SA_ERR_TYPE_FUNCTION_ALREADY_DECLARED,
      SA_ERR_TYPE_STRUCTURE_ALREADY_DEFINED,
      SA_ERR_TYPE_UNION_ALREADY_DEFINED,
      SA_ERR_TYPE_ENUM_ALREADY_DEFINED,

      // 1
      SA_ERR_TYPE_SIZE_IS_NOT_KNOWN_AT_COMPTIME,

      // 1
      SA_ERR_TYPE_ARRAY_NOT_RAW_INT,
      SA_ERR_TYPE_ARRAY_PREFIXOP_IS_NOT_COMPTIME,
      SA_ERR_TYPE_ARRAY_EXPR_IS_NOT_COMPTIME,

      // 1
      SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_INTLIT,
      SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_FLOATLIT,
      SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_BOOLLIT,
      SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_ENUMLIT,
      SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_INTLIT,
      SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_FLOATLIT,
      SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_BOOLLIT,
      SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_ENUMLIT,

      // 1
      SA_ERR_TYPE_CANNOT_CAST_VALUE_TO_AN_ARRAY,

      // 1
      SA_ERR_TYPE_VARDEFS_IN_FUNCPTR_ARGS_ARE_NOT_ALLOWED,

      // 1
      SA_ERR_TYPE_CANNOT_ASSIGN_NONPOINTER_VALUE_TO_A_POINTER,
    };

    std::vector<size_t> positions;
    ErrType type;

    SAError (size_t token_position, ErrType type) : type (type)
    {
      positions.push_back (token_position);
    }

    SAError (std::vector<size_t> token_positions, ErrType type)
        : positions (std::move (token_positions)), type (type)
    {
    }
  };

  const std::vector<SAError> &get_errors () const;

private:
  AST _root;
  std::vector<SAError> _errors{};
  ScopeStack _scope_stack{};

  void analyze_program (const AST &prog);
  void analyze_vardecl (const AST &vardecl);
  void analyze_vardef (const AST &vardef);

  Type get_type_from_type_ast (const AST &type_ast);
  Type get_type_from_expr_ast (const AST &expr_ast);
  Type resolve_plain_type (const std::string &type, size_t plain_type_pos);

  std::vector<Type>
  get_types_of_args_from_funcptr_arglist (const AST &arglist);

  bool variable_is_defined (const std::string &name);
  bool function_is_defined (const std::string &name);
  Type search_for_variable_type (const std::string &name, bool &found);

  size_t get_size_of_builtin_type (BuiltinType type);
  bool verify_comptime_array (const AST &type_ast);
  bool verify_expr_comptime (const AST &expr_ast);

  bool is_raw_integer (const Type &type);

  bool is_value_boollit (const std::string &str) const;

  const std::unordered_map<std::string, BuiltinType> typestr_to_builtin_type
      = {
          { "void", BuiltinType::BUILTIN_TYPE_VOID },

          { "char", BuiltinType::BUILTIN_TYPE_CHAR },
          { "short", BuiltinType::BUILTIN_TYPE_SHORT },
          { "int", BuiltinType::BUILTIN_TYPE_INT },
          { "long", BuiltinType::BUILTIN_TYPE_LONG },

          { "uchar", BuiltinType::BUILTIN_TYPE_UCHAR },
          { "ushort", BuiltinType::BUILTIN_TYPE_USHORT },
          { "uint", BuiltinType::BUILTIN_TYPE_UINT },
          { "ulong", BuiltinType::BUILTIN_TYPE_ULONG },

          { "float", BuiltinType::BUILTIN_TYPE_FLOAT },
          { "double", BuiltinType::BUILTIN_TYPE_DOUBLE },

          { "bool", BuiltinType::BUILTIN_TYPE_BOOL },
        };
  bool is_string_a_builtin_type (const std::string &str) const;

  const std::unordered_map<std::string, BuiltinType>
      typespecstr_to_builtin_type = {
        { "hhu", BuiltinType::BUILTIN_TYPE_UCHAR },
        { "hu", BuiltinType::BUILTIN_TYPE_USHORT },
        { "lu", BuiltinType::BUILTIN_TYPE_UINT },
        { "u", BuiltinType::BUILTIN_TYPE_UINT },
        { "llu", BuiltinType::BUILTIN_TYPE_ULONG },

        { "hh", BuiltinType::BUILTIN_TYPE_CHAR },
        { "h", BuiltinType::BUILTIN_TYPE_SHORT },
        { "l", BuiltinType::BUILTIN_TYPE_INT },
        { "ll", BuiltinType::BUILTIN_TYPE_LONG },

        { "f", BuiltinType::BUILTIN_TYPE_FLOAT },
        { "lf", BuiltinType::BUILTIN_TYPE_DOUBLE },
      };
  bool is_string_a_typespec (const std::string &str) const;

  bool can_convert_type (const Type &from, const Type &to) const;
  BuiltinType get_converted_type (BuiltinType left, BuiltinType right) const;

  bool is_type_is_integer (const Type &t) const;
  bool is_type_is_float (const Type &t) const;

  template <typename... Args>
  void
  add_error (Args &&...args)
  {
    _errors.emplace_back (args...);
  }
};

}
