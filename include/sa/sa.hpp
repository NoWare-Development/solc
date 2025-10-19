#pragma once

#include <cstdint>
#include <memory>
#include <parser/ast.hpp>
#include <sa/scope.hpp>
#include <string>
#include <symbols/symbol.hpp>
#include <symbols/symbolscope.hpp>
#include <types.hpp>
#include <unordered_map>

namespace solc
{

class SemanticAnalyzer {
  public:
  SemanticAnalyzer();
  ~SemanticAnalyzer() = default;

  void analyze(const AST &root);

  enum class SAErrorType {
    NONE,

    UNDEFINED_TYPE,

    SYMBOL_REDEF,
    TYPE_REDEF,
    VAR_REDECL,

    ARRAY_IN_TYPEDEF,

    INVALID_TYPESPEC,
    INT_TYPESPEC_ON_FLOAT,

    ID_IN_CONST_EXPR,
    STRING_IN_CONST_EXPR,
    NAMESPACE_IN_CONST_EXPR,
    CALL_IN_CONST_EXPR,
    ARRAY_ELEM_IN_CONST_EXPR,
    ACCESS_MEMBER_IN_CONST_EXPR,

    TAKE_ADDRESS_IN_CONST_EXPR,
    DEREF_IN_CONST_EXPR,

    CANNOT_CAST,
    CANNOT_CONVERT_TYPES_IN_EXPR,
    BITWISE_OP_ON_FLOAT,

    TAKE_ADDRESS_OF_A_LITERAL,
    DEREF_ON_NONPTR,
    TOO_BIG_PTR_INDIRECTION,

    NEG_ON_NONNUM,
    BINOP_ON_NONNUM,

    NONINT_IN_ARRAY_DECL,

    ENUM_IS_EMPTY,
    ENUM_ELEM_EXPR_NOT_INT,
    ENUM_ELEM_REDEF,
  };
  struct SAError {
    std::vector<std::shared_ptr<Type> > types{};
    std::vector<size_t> token_positions{};
    std::vector<long long> expected_ints{};

    SAErrorType type = SAErrorType::NONE;

    SAError() = default;
    SAError(SAErrorType type, size_t tok_pos,
            std::vector<std::shared_ptr<Type> > types = {})
      : types(std::move(types))
      , type(type)
    {
      token_positions.push_back(tok_pos);
    }
    SAError(SAErrorType type, std::vector<size_t> token_positions,
            std::vector<std::shared_ptr<Type> > types = {})
      : types(std::move(types))
      , token_positions(std::move(token_positions))
      , type(type)
    {
    }
    SAError(SAErrorType type, size_t tok_pos,
            std::vector<long long> expected_ints,
            std::vector<std::shared_ptr<Type> > types = {})
      : types(std::move(types))
      , expected_ints(std::move(expected_ints))
      , type(type)
    {
      token_positions.push_back(tok_pos);
    }
    SAError(SAErrorType type, std::vector<size_t> token_positions,
            std::vector<long long> expected_ints,
            std::vector<std::shared_ptr<Type> > types = {})
      : types(std::move(types))
      , token_positions(std::move(token_positions))
      , expected_ints(std::move(expected_ints))
      , type(type)
    {
    }

    static SAError success()
    {
      return SAError();
    }
  };

  const std::vector<SAError> &get_errors() const;

  private:
  ScopeStack *_scope_stack{};

  // NOTE: using `void *` as a key to have access to scopes without
  std::unordered_map<void *, SymbolScope> _symbol_scope_bindings{};
  std::vector<std::pair<Struct *, const AST *> > struct_asts{};
  std::vector<std::pair<Union *, const AST *> > union_asts{};
  std::vector<std::pair<Enum *, const AST *> > enum_asts{};
  std::vector<std::pair<Function *, const AST *> > func_asts{};
  SymbolScope *_current_symbol_scope{};
  SymbolScope *_global_symbol_scope{};

  const std::unordered_map<std::string, std::shared_ptr<Type> > _basic_types{
    { "void", Type::create_basic(BuiltinType::VOID) },

    { "uchar", Type::create_basic(BuiltinType::UCHAR) },
    { "ushort", Type::create_basic(BuiltinType::USHORT) },
    { "uint", Type::create_basic(BuiltinType::UINT) },
    { "ulong", Type::create_basic(BuiltinType::ULONG) },

    { "char", Type::create_basic(BuiltinType::CHAR) },
    { "short", Type::create_basic(BuiltinType::SHORT) },
    { "int", Type::create_basic(BuiltinType::INT) },
    { "long", Type::create_basic(BuiltinType::LONG) },

    { "float", Type::create_basic(BuiltinType::FLOAT) },
    { "double", Type::create_basic(BuiltinType::DOUBLE) },

    { "bool", Type::create_basic(BuiltinType::BOOL) },
  };

  const std::unordered_map<std::string, std::shared_ptr<Type> >
    _typespec_to_type{
      { "hh", _basic_types.at("char") },   { "hhi", _basic_types.at("char") },
      { "h", _basic_types.at("short") },   { "hi", _basic_types.at("short") },
      { "i", _basic_types.at("int") },     { "l", _basic_types.at("long") },
      { "li", _basic_types.at("long") },

      { "hhu", _basic_types.at("uchar") }, { "hu", _basic_types.at("ushort") },
      { "u", _basic_types.at("uint") },    { "lu", _basic_types.at("ulong") },

      { "f", _basic_types.at("float") },   { "d", _basic_types.at("double") },
      { "lf", _basic_types.at("double") },
    };

  std::unordered_map<std::string, std::shared_ptr<Type> >
    _architecture_dependent_types{};
  const std::unordered_map<std::string, std::shared_ptr<Type> > _literal_type{
    { "true", _basic_types.at("bool") },
    { "false", _basic_types.at("bool") },
    { "nullptr", Type::create_basic(BuiltinType::VOID, 1) },
  };

  void populate_architecture_dependent_types();

  std::vector<SAError> _errors{};

  bool is_basic_type(const std::string &type) const;
  std::shared_ptr<Type> get_basic_type(const std::string &type) const;

  bool is_valid_typespec(const std::string &ts) const;
  std::shared_ptr<Type> get_typespec(const std::string &ts) const;

  bool is_valid_literal(const std::string &literal) const;
  std::shared_ptr<Type> get_literal_type(const std::string &literal) const;

  bool does_type_exist(const std::string &type) const;

  void analyze_prog(const AST &prog);
  void analyze_typedef(const AST &tdef);
  void
  analyze_vardecl(const AST &vardecl,
                  ModifierFlags flags = ModifierFlagBits::MODIFIER_FLAG_NONE);
  void
  analyze_vardef(const AST &vardef,
                 ModifierFlags flags = ModifierFlagBits::MODIFIER_FLAG_NONE);
  void analyze_structdef(const AST &structdef);

  std::shared_ptr<Type> get_type_from_type_ast(const AST &type);

  SAError is_expr_known_at_comptime(const AST &expr) const;
  std::shared_ptr<Type> get_type_from_expr_ast(const AST &expr,
                                               bool taking_address = false);
  size_t get_expr_comptime_ivalue(const AST &expr) const;

  bool verify_type_of_initlist(const AST &initlist,
                               const std::shared_ptr<Type> &type,
                               size_t type_tok_pos = -1);

  void generate_symbol_scope_bindings(const AST &root);
  void generate_aliases(const AST &root);

  void populate_symbols();
  void populate_enums(const std::vector<std::pair<Enum *, const AST *> > asts);
  void
  populate_structs(const std::vector<std::pair<Struct *, const AST *> > asts);
  void
  populate_unions(const std::vector<std::pair<Union *, const AST *> > asts);
  void
  populate_funcs(const std::vector<std::pair<Function *, const AST *> > asts);

  std::shared_ptr<Type> resolve_type(const std::string &name) const;
  std::shared_ptr<Type> resolve_type(const std::vector<std::string> &ns,
                                     const std::string &name) const;

  template <typename... Args> void add_error(Args &&...args)
  {
    _errors.emplace_back(args...);
  }
};

}
