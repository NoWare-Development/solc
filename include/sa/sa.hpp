#pragma once

#include <memory>
#include <sa/scope.hpp>
#include <symbols/symboltable.hpp>
#include <types.hpp>

namespace nlc
{

class SemanticAnalyzer
{
public:
  SemanticAnalyzer ();
  ~SemanticAnalyzer () = default;

  std::unique_ptr<SymbolTable> analyze (const AST &root);

  enum class SAErrorType
  {
    UNDEFINED_TYPE,
    TYPE_REDEF,
  };
  struct SAError
  {
    SAErrorType type;
    size_t tok_pos;

    SAError (SAErrorType type, size_t tok_pos) : type (type), tok_pos (tok_pos)
    {
    }
  };

  const std::vector<SAError> &get_errors () const;

private:
  ScopeStack *_scope_stack{};
  std::unique_ptr<SymbolTable> _symbol_table{};

  const std::unordered_map<std::string, std::shared_ptr<Type>> _basic_types{
    { "void", Type::create_basic (BuiltinType::VOID) },

    { "uchar", Type::create_basic (BuiltinType::UCHAR) },
    { "ushort", Type::create_basic (BuiltinType::USHORT) },
    { "uint", Type::create_basic (BuiltinType::UINT) },
    { "ulong", Type::create_basic (BuiltinType::ULONG) },

    { "char", Type::create_basic (BuiltinType::CHAR) },
    { "short", Type::create_basic (BuiltinType::SHORT) },
    { "int", Type::create_basic (BuiltinType::INT) },
    { "long", Type::create_basic (BuiltinType::LONG) },

    { "float", Type::create_basic (BuiltinType::FLOAT) },
    { "double", Type::create_basic (BuiltinType::DOUBLE) },

    { "bool", Type::create_basic (BuiltinType::BOOL) },
  };

  std::unordered_map<std::string, std::shared_ptr<Type>>
      _architecture_dependent_types{};

  void populate_architecture_dependent_types ();

  std::vector<SAError> _errors{};

  bool is_basic_type (const std::string &type) const;
  std::shared_ptr<Type> get_basic_type (const std::string &type);

  bool does_type_exist (const std::string &type) const;

  void analyze_prog (const AST &prog);

  void analyze_typedef (const AST &tdef);

  std::shared_ptr<Type> get_type_from_type_ast (const AST &type);

  template <typename... Args>
  void
  add_error (Args &&...args)
  {
    _errors.emplace_back (args...);
  }
};

}
