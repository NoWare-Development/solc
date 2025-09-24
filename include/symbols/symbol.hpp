#pragma once

#include <cstdint>
#include <memory>
#include <parser/ast.hpp>
#include <string>
#include <types.hpp>
#include <unordered_map>

namespace nlc
{

struct SymbolData
{
  virtual ~SymbolData () = default;
};

enum ModifierFlagBits : uint8_t
{
  MODIFIER_FLAG_NONE = 0,
  MODIFIER_FLAG_CONST = 1 << 0,
  MODIFIER_FLAG_STATIC = 1 << 1,
  MODIFIER_FLAG_INLINE = 1 << 2,
};

using ModifierFlags = uint8_t;

struct Variable : SymbolData
{
  std::shared_ptr<Type> type;
  ModifierFlags flags;
  bool defined;

  Variable (std::shared_ptr<Type> type, bool defined, ModifierFlags flags)
      : type (type), flags (flags), defined (defined)
  {
  }
  virtual ~Variable () override = default;

  static std::shared_ptr<Variable>
  create (std::shared_ptr<Type> type, bool defined,
          ModifierFlags flags = MODIFIER_FLAG_NONE)
  {
    return std::make_shared<Variable> (type, defined, flags);
  }
};

struct Function : SymbolData
{
  struct Arg
  {
    std::shared_ptr<Type> type;
    std::string name;
    ModifierFlags flags;

    static Arg
    create (std::shared_ptr<Type> type, const std::string &name,
            ModifierFlags flags = MODIFIER_FLAG_NONE)
    {
      return { .type = type, .name = name, .flags = flags };
    }
  };

  std::vector<Arg> arguments{};
  std::shared_ptr<Type> return_type;
  ModifierFlags flags;

  Function (std::shared_ptr<Type> return_type, ModifierFlags flags)
      : return_type (return_type), flags (flags)
  {
  }
  Function (std::vector<Arg> &&arguments, ModifierFlags flags)
      : arguments (arguments),
        return_type (Type::create_basic (BuiltinType::VOID)), flags (flags)
  {
  }
  Function (std::shared_ptr<Type> return_type, std::vector<Arg> &&arguments,
            ModifierFlags flags)
      : arguments (arguments), return_type (return_type), flags (flags)
  {
  }
  virtual ~Function () override = default;

  static std::shared_ptr<Function>
  create (ModifierFlags flags = MODIFIER_FLAG_NONE)
  {
    return std::make_shared<Function> (Type::create_basic (BuiltinType::VOID),
                                       flags);
  }
  static std::shared_ptr<Function>
  create (std::shared_ptr<Type> return_type,
          ModifierFlags flags = MODIFIER_FLAG_NONE)
  {
    return std::make_shared<Function> (return_type, flags);
  }
  static std::shared_ptr<Function>
  create (std::vector<Arg> arguments, ModifierFlags flags = MODIFIER_FLAG_NONE)
  {
    return std::make_shared<Function> (std::move (arguments), flags);
  }
  static std::shared_ptr<Function>
  create (std::shared_ptr<Type> return_type, std::vector<Arg> arguments,
          ModifierFlags flags = MODIFIER_FLAG_NONE)
  {
    return std::make_shared<Function> (return_type, std::move (arguments),
                                       flags);
  }
};

struct Enum : SymbolData
{
  struct Element
  {
    std::shared_ptr<AST> expr_ast;
    size_t position;

    static Element
    create ()
    {
      return { .expr_ast = {}, .position = 0 };
    }

    static Element
    create (std::shared_ptr<AST> expr_ast)
    {
      return { .expr_ast = expr_ast, .position = 0 };
    }

    static Element
    create (const Element &e)
    {
      if (e.expr_ast != nullptr)
        {
          AST add_op (e.expr_ast->token_position,
                      ASTType::EXPR_BINARY_OPERATOR_ADD);
          AST num_1 (e.expr_ast->token_position, ASTType::EXPR_OPERAND_NUM,
                     "1");
          auto new_expr = std::make_shared<AST> (e.expr_ast->token_position,
                                                 ASTType::EXPR);
          new_expr->append (*e.expr_ast);
          new_expr->append (add_op);
          new_expr->append (num_1);
          return { .expr_ast = new_expr, .position = 0 };
        }

      return { .expr_ast = nullptr, .position = e.position + 1 };
    }
  };

  std::unordered_map<std::string, Element> elements{};

  Enum () = default;
  Enum (std::unordered_map<std::string, Element> &&elements)
      : elements (elements)
  {
  }
  virtual ~Enum () override = default;

  static std::shared_ptr<Enum>
  create ()
  {
    return std::make_shared<Enum> ();
  }

  static std::shared_ptr<Enum>
  create (std::unordered_map<std::string, Element> elements)
  {
    return std::make_shared<Enum> (std::move (elements));
  }

  bool has_element (const std::string &name) const;
  void add_element (const std::string &name, Element &&element);
  const Element &get_element (const std::string &name) const;
};

struct Struct : SymbolData
{
  std::unordered_map<std::string, std::shared_ptr<Variable>> fields{};

  Struct () = default;
  Struct (std::unordered_map<std::string, std::shared_ptr<Variable>> &&fields)
      : fields (fields)
  {
  }
  virtual ~Struct () override = default;

  static std::shared_ptr<Struct>
  create ()
  {
    return std::make_shared<Struct> ();
  }

  static std::shared_ptr<Struct>
  create (std::unordered_map<std::string, std::shared_ptr<Variable>> fields)
  {
    return std::make_shared<Struct> (std::move (fields));
  }

  bool has_field (const std::string &name) const;
  void add_field (const std::string &name, std::shared_ptr<Variable> field);
  const std::shared_ptr<Variable> &get_field (const std::string &name) const;
};

struct Union : SymbolData
{
  std::unordered_map<std::string, std::shared_ptr<Variable>> fields{};

  Union () = default;
  Union (std::unordered_map<std::string, std::shared_ptr<Variable>> &&fields)
      : fields (fields)
  {
  }
  virtual ~Union () override = default;

  static std::shared_ptr<Union>
  create ()
  {
    return std::make_shared<Union> ();
  }

  static std::shared_ptr<Union>
  create (std::unordered_map<std::string, std::shared_ptr<Variable>> fields)
  {
    return std::make_shared<Union> (std::move (fields));
  }

  bool has_field (const std::string &name) const;
  void add_field (const std::string &name, std::shared_ptr<Variable> field);
  const std::shared_ptr<Variable> &get_field (const std::string &name) const;
};

}
