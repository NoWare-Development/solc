#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <parser/ast.hpp>
#include <string>
#include <types.hpp>
#include <unordered_map>

namespace nlc
{

struct SymbolScope;

struct Symbol
{
  friend struct SymbolScope;

  virtual ~Symbol () = default;

  enum class Type
  {
    ALIAS,
    VARIABLE,
    FUNCTION,
    STRUCT,
    ENUM,
    UNION,
  };

  virtual Type get_symbol_type () const = 0;

  const std::string &
  get_symbol_name () const
  {
    return _symbol_name;
  }

  const SymbolScope *
  get_symbol_scope () const
  {
    return _symbol_scope;
  }

private:
  std::string _symbol_name{};
  SymbolScope *_symbol_scope{};
};

enum ModifierFlagBits : uint8_t
{
  MODIFIER_FLAG_NONE = 0,
  MODIFIER_FLAG_CONST = 1 << 0,
  MODIFIER_FLAG_STATIC = 1 << 1,
  MODIFIER_FLAG_INLINE = 1 << 2,
};

using ModifierFlags = uint8_t;

struct Alias : Symbol
{
  std::shared_ptr<nlc::Type> type;

  Alias () = default;
  virtual ~Alias () override = default;

  Alias (std::shared_ptr<nlc::Type> type) : type (type) {}

  static std::shared_ptr<Alias>
  create (std::shared_ptr<nlc::Type> type)
  {
    return std::make_shared<Alias> (type);
  }

  std::shared_ptr<nlc::Type>
  get_type ()
  {
    return type;
  }

  virtual Type
  get_symbol_type () const override
  {
    return Type::ALIAS;
  }
};

struct Variable : Symbol
{
  std::shared_ptr<nlc::Type> type;
  ModifierFlags flags;
  bool defined;

  Variable (std::shared_ptr<nlc::Type> type, bool defined, ModifierFlags flags)
      : type (type), flags (flags), defined (defined)
  {
  }
  virtual ~Variable () override = default;

  static std::shared_ptr<Variable>
  create (std::shared_ptr<nlc::Type> type, bool defined,
          ModifierFlags flags = MODIFIER_FLAG_NONE)
  {
    return std::make_shared<Variable> (type, defined, flags);
  }

  virtual Type
  get_symbol_type () const override
  {
    return Type::VARIABLE;
  }
};

struct Function : Symbol
{
  struct Arg
  {
    std::shared_ptr<nlc::Type> type;
    std::string name;
    ModifierFlags flags;

    static Arg
    create (std::shared_ptr<nlc::Type> type, const std::string &name,
            ModifierFlags flags = MODIFIER_FLAG_NONE)
    {
      return { .type = type, .name = name, .flags = flags };
    }
  };

  std::vector<Arg> arguments;
  std::shared_ptr<nlc::Type> return_type;
  ModifierFlags flags;

  Function () = default;
  virtual ~Function () override = default;

  static std::shared_ptr<Function>
  create ()
  {
    return std::make_shared<Function> ();
  }

  void
  set_modifier_flags (ModifierFlags flags)
  {
    this->flags = flags;
  }

  void
  set_arguments (std::vector<Arg> arguments)
  {
    this->arguments = arguments;
  }

  void
  set_return_type (std::shared_ptr<nlc::Type> return_type)
  {
    this->return_type = return_type;
  }

  virtual Type
  get_symbol_type () const override
  {
    return Type::FUNCTION;
  }
};

struct Enum : Symbol
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
  virtual ~Enum () override = default;

  static std::shared_ptr<Enum>
  create ()
  {
    return std::make_shared<Enum> ();
  }

  bool
  has_element (const std::string &name) const
  {
    return elements.find (name) != elements.end ();
  }

  void
  add_element (const std::string &name, Element &&element)
  {
    elements[name] = element;
  }

  const Element &
  get_element (const std::string &name) const
  {
    return elements.at (name);
  }

  virtual Type
  get_symbol_type () const override
  {
    return Type::ENUM;
  }
};

struct Union;

struct Struct : Symbol
{
  struct PositionedType
  {
    std::shared_ptr<nlc::Type> handle;
    size_t pos;

    PositionedType () = default;
    PositionedType (std::shared_ptr<nlc::Type> handle, size_t position)
        : handle (handle), pos (position)
    {
    }
  };

  std::unordered_map<std::string, PositionedType> fields{};
  std::unordered_map<std::string, std::shared_ptr<Alias>> aliases{};
  std::unordered_map<std::string, std::shared_ptr<Function>> methods{};
  std::unordered_map<std::string, std::shared_ptr<Struct>> structs{};
  std::unordered_map<std::string, std::shared_ptr<Union>> unions{};
  std::unordered_map<std::string, std::shared_ptr<Enum>> enums{};

  Struct () = default;
  virtual ~Struct () override = default;

  static std::shared_ptr<Struct>
  create ()
  {
    return std::make_shared<Struct> ();
  }

  bool
  has_field (const std::string &name) const
  {
    return fields.find (name) != fields.end ();
  }

  void
  add_field (const std::string &name, PositionedType &&field)
  {
    fields[name] = field;
  }

  std::shared_ptr<nlc::Type>
  get_field (const std::string &name)
  {
    return fields.at (name).handle;
  }

  std::vector<std::shared_ptr<nlc::Type>>
  get_fields_sorted ()
  {
    std::vector<PositionedType> postypes{};
    for (auto &kv : fields)
      postypes.push_back (kv.second);

    std::sort (postypes.begin (), postypes.end (),
               [] (const PositionedType &a, const PositionedType &b) {
                 return a.pos < b.pos;
               });

    std::vector<std::shared_ptr<nlc::Type>> out{};
    for (auto &t : postypes)
      out.push_back (t.handle);

    return out;
  }

  bool
  has_alias (const std::string &name) const
  {
    return aliases.find (name) != aliases.end ();
  }

  void
  add_alias (const std::string &name, std::shared_ptr<Alias> alias)
  {
    aliases[name] = alias;
  }

  std::shared_ptr<Alias>
  get_alias (const std::string &name)
  {
    return aliases.at (name);
  }

  bool
  has_method (const std::string &name) const
  {
    return methods.find (name) != methods.end ();
  }

  void
  add_method (const std::string &name, std::shared_ptr<Function> method)
  {
    methods[name] = method;
  }

  std::shared_ptr<Function>
  get_method (const std::string &name)
  {
    return methods.at (name);
  }

  bool
  has_struct (const std::string &name) const
  {
    return structs.find (name) != structs.end ();
  }

  void
  add_struct (const std::string &name, std::shared_ptr<Struct> s)
  {
    structs[name] = s;
  }

  std::shared_ptr<Struct>
  get_struct (const std::string &name)
  {
    return structs.at (name);
  }

  bool
  has_enum (const std::string &name) const
  {
    return enums.find (name) != enums.end ();
  }

  void
  add_enum (const std::string &name, std::shared_ptr<Enum> e)
  {
    enums[name] = e;
  }

  std::shared_ptr<Enum>
  get_enum (const std::string &name)
  {
    return enums.at (name);
  }

  bool
  has_union (const std::string &name) const
  {
    return unions.find (name) != unions.end ();
  }

  void
  add_union (const std::string &name, std::shared_ptr<Union> u)
  {
    unions[name] = u;
  }

  std::shared_ptr<Union>
  get_union (const std::string &name)
  {
    return unions.at (name);
  }

  virtual Type
  get_symbol_type () const override
  {
    return Type::STRUCT;
  }
};

struct Union : Struct
{
  Union () = default;
  virtual ~Union () override = default;

  static std::shared_ptr<Union>
  create ()
  {
    return std::make_shared<Union> ();
  }

  virtual Type
  get_symbol_type () const override
  {
    return Type::UNION;
  }
};

}
