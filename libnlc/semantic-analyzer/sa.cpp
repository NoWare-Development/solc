#include "semantic-analyzer/sa.hpp"
#include "libnlc/util.hpp"
#include "parser/ast.hpp"
#include "semantic-analyzer/scope.hpp"
#include "semantic-analyzer/types.hpp"

namespace nlc::sa
{

bool
SemanticAnalyzer::analyze ()
{
  if (_root.type != ASTType::AST_PROG)
    {
      add_error (_root.token_position,
                 SAError::ErrType::SA_ERR_TYPE_WRONG_AST_TYPE);
      return false;
    }
  analyze_program (_root);

  return _errors.size () == 0;
}

const std::vector<SemanticAnalyzer::SAError> &
SemanticAnalyzer::get_errors () const
{
  return _errors;
}

void
SemanticAnalyzer::analyze_program (const AST &prog)
{
  Scope global_scope{};
  global_scope.kind = ScopeKind::SCOPE_KIND_GLOBAL;
  _scope_stack.push (global_scope);

  const auto &children = prog.children;
  for (const auto &child : children)
    {
      if (child.type == ASTType::AST_VAR_DEF)
        {
          analyze_vardef (child);
          continue;
        }

      TODO ("More AST types");
    }
}

#ifndef IS_IN_MAP
#define IS_IN_MAP(map, val) ((map).find ((val)) != (map).end ())
#endif

bool
SemanticAnalyzer::variable_is_defined (const std::string &name)
{
  for (const auto &scope : _scope_stack)
    {
      if (IS_IN_MAP (scope.variable_table, name))
        {
          return true;
        }
    }
  return false;
}

Type
SemanticAnalyzer::search_for_variable_type (const std::string &name,
                                            bool &found)
{
  Type out{};

  found = false;
  for (const auto &scope : _scope_stack)
    {
      if (IS_IN_MAP (scope.variable_table, name))
        {
          out = scope.variable_table.at (name).type;
          found = true;
          break;
        }
    }

  return out;
}

size_t
SemanticAnalyzer::get_size_of_builtin_type (BuiltinType type)
{
  static const size_t sizes[17] = {
    0, // UNK

    0, // VOID

    1, // UCHAR
    2, // USHORT
    4, // UINT
    8, // ULONG

    1, // CHAR
    2, // SHORT
    4, // INT
    8, // LONG

    4, // FLOAT
    8, // DOUBLE

    1, // BOOL

    0, // UNION
    0, // STRUCT
    0, // ENUM

    0, // FUNC
  };

  return sizes[type];
}

bool
SemanticAnalyzer::is_string_a_builtin_type (const std::string &str) const
{
  return IS_IN_MAP (typestr_to_builtin_type, str);
}

bool
SemanticAnalyzer::is_string_a_typespec (const std::string &str) const
{
  return IS_IN_MAP (typespecstr_to_builtin_type, str);
}

bool
SemanticAnalyzer::can_convert_type (const Type &from, const Type &to) const
{
  if (from.type == to.type && from.type_name == to.type_name
      && from.pointer_count == to.pointer_count)
    return true;

  if (from.pointer_count > 0 || to.pointer_count > 0)
    {
      if (from.pointer_count > 0)
        return is_type_is_integer (to);
      else if (to.pointer_count > 0)
        return is_type_is_integer (from);
    }

  if (get_converted_type (to.type, from.type) != BuiltinType::BUILTIN_TYPE_UNK)
    return true;

  return false;
}

BuiltinType
SemanticAnalyzer::get_converted_type (BuiltinType left,
                                      BuiltinType right) const
{
#define _un BuiltinType::BUILTIN_TYPE_UNK

#define _vd BuiltinType::BUILTIN_TYPE_VOID

#define _uc BuiltinType::BUILTIN_TYPE_UCHAR
#define _us BuiltinType::BUILTIN_TYPE_USHORT
#define _ui BuiltinType::BUILTIN_TYPE_UINT
#define _ul BuiltinType::BUILTIN_TYPE_ULONG

#define _sc BuiltinType::BUILTIN_TYPE_CHAR
#define _ss BuiltinType::BUILTIN_TYPE_SHORT
#define _si BuiltinType::BUILTIN_TYPE_INT
#define _sl BuiltinType::BUILTIN_TYPE_LONG

#define _fl BuiltinType::BUILTIN_TYPE_FLOAT
#define _dl BuiltinType::BUILTIN_TYPE_DOUBLE

#define _bl BuiltinType::BUILTIN_TYPE_BOOL

#define _uo BuiltinType::BUILTIN_TYPE_UNION
#define _st BuiltinType::BUILTIN_TYPE_STRUCT
#define _en BuiltinType::BUILTIN_TYPE_ENUM

#define _fn BuiltinType::BUILTIN_TYPE_FUNC

  // Scary table
  // Horrors of compiler development
  static const BuiltinType conversion_table[17][17] = {
    { _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un,
      _un, _un, _un },
    { _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un,
      _un, _un, _un },
    { _un, _un, _uc, _us, _ui, _ul, _uc, _ss, _si, _sl, _fl, _dl, _uc, _un,
      _un, _uc, _fn },
    { _un, _un, _us, _us, _ui, _ul, _us, _us, _si, _sl, _fl, _dl, _us, _un,
      _un, _us, _un },
    { _un, _un, _ui, _ui, _ui, _ul, _ui, _ui, _ui, _sl, _fl, _dl, _ui, _un,
      _un, _ui, _un },
    { _un, _un, _ul, _ul, _ul, _ul, _ul, _ul, _ul, _ul, _fl, _dl, _ul, _un,
      _un, _ul, _un },
    { _un, _un, _uc, _us, _ui, _ul, _sc, _ss, _si, _sl, _fl, _dl, _sc, _un,
      _un, _sc, _un },
    { _un, _un, _ss, _us, _ui, _ul, _ss, _ss, _si, _sl, _fl, _dl, _ss, _un,
      _un, _ss, _un },
    { _un, _un, _si, _ss, _ui, _ul, _si, _si, _si, _sl, _fl, _dl, _si, _un,
      _un, _si, _un },
    { _un, _un, _sl, _sl, _sl, _ul, _sl, _sl, _sl, _sl, _fl, _dl, _sl, _un,
      _un, _sl, _un },
    { _un, _un, _fl, _fl, _fl, _fl, _fl, _fl, _fl, _fl, _fl, _dl, _fl, _un,
      _un, _fl, _un },
    { _un, _un, _dl, _dl, _dl, _dl, _dl, _dl, _dl, _dl, _dl, _dl, _dl, _un,
      _un, _dl, _un },
    { _un, _un, _uc, _us, _ui, _ul, _sc, _ss, _si, _sl, _fl, _dl, _bl, _un,
      _un, _ui, _un },
    { _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _uo,
      _un, _un, _un },
    { _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un,
      _st, _un, _un },
    { _un, _un, _uc, _us, _ui, _ul, _sc, _ss, _si, _sl, _fl, _dl, _ui, _un,
      _un, _ui, _un },
    { _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un, _un,
      _un, _un, _un },
  };

  return conversion_table[left][right];
}

bool
SemanticAnalyzer::is_type_is_integer (const Type &t) const
{
  if (t.pointer_count > 0)
    return true;

  switch (t.type)
    {
    case BuiltinType::BUILTIN_TYPE_UCHAR:
    case BuiltinType::BUILTIN_TYPE_USHORT:
    case BuiltinType::BUILTIN_TYPE_UINT:
    case BuiltinType::BUILTIN_TYPE_ULONG:
    case BuiltinType::BUILTIN_TYPE_CHAR:
    case BuiltinType::BUILTIN_TYPE_SHORT:
    case BuiltinType::BUILTIN_TYPE_INT:
    case BuiltinType::BUILTIN_TYPE_LONG:
    case BuiltinType::BUILTIN_TYPE_BOOL:
    case BuiltinType::BUILTIN_TYPE_ENUM:
      return true;

    default:
      break;
    }

  return false;
}

bool
SemanticAnalyzer::is_type_is_float (const Type &t) const
{
  if (t.pointer_count > 0)
    return false;

  switch (t.type)
    {
    case BuiltinType::BUILTIN_TYPE_FLOAT:
    case BuiltinType::BUILTIN_TYPE_DOUBLE:
      return true;

    default:
      break;
    }

  return false;
}

}
