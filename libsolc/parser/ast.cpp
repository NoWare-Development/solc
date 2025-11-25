#include "parser/ast.hpp"
#include <cstdint>

namespace solc
{

static std::string asttype_to_string(ASTType type);

std::string AST::to_string() const
{
  // Recursively loop through all children in AST
  // and format output accordingly.
  std::string out{};

  if (_depth > 0) {
    for (size_t i = 0; i < _depth; i++) {
      out += "  ";
    }

    out += "(" + std::to_string(_depth) + ") ";
  }

  out += asttype_to_string(type) + " \"" + value + "\"";

  if (!children.empty()) {
    for (auto &child : children) {
      out += '\n' + child.to_string();
    }
  }

  return out;
}

bool AST::is_empty() const
{
  return children.empty();
}

void AST::set_depth(size_t depth)
{
  _depth = depth;
  for (auto &child : children) {
    child.set_depth(depth + 1);
  }
}

AST *AST::append(AST child)
{
  child.set_depth(_depth + 1);
  children.push_back(child);
  return &children.at(children.size() - 1);
}

#define __AST_GET_GROUP(type) (((uint16_t)(type) & 0xFF00) >> 8)
#define __AST_GET_ID(type) ((uint16_t)(type) & 0xFF)

static std::string asttype_to_string(ASTType type)
{
  constexpr const char *groups[0x100] = {
    "",
    "STMT",
    "ENUM",
    "TYPE",
    "VAR",
    "FUNC",
    "EXPR_BINARY_OPERATOR",
    "EXPR_COMPARE_OPERATOR",
    "EXPR_BOOLEAN_OPERATOR",
    "EXPR_ASSIGN_OPERATOR",
    "EXPR_PREFIX_OPERATOR",
    "EXPR_OPERAND",
    "VISIBILITY_MARKER",
    "INITLIST",
    "GENERIC",
  };

  constexpr const char *ids[0x100][0x100] = {
    {
      // Groupless
      "ERR",

      "PROG",          "EXPR",        "MODULE",      "IMPORT",  "TYPEDEF",
      "EXPORTED_FUNC", "EXTERN_FUNC", "MODIFIER",    "NONE",    "LABEL",
      "VARIADIC",      "STRUCT",      "UNION",       "ENUM",    "FUNC",
      "NAMESPACE",     "INITLIST",    "PREFIX_EXPR", "GENERIC",
    },
    {
      // STMT
      "LIST",
      "RETURN",
      "GOTO",
      "LABEL",
      "BREAK",
      "CONTINUE",
      "WHILE",
      "FOR",
      "DOWHILE",
      "EXPR",
      "SWITCH",
      "CASE",
      "DEFAULT",
      "DEFER",
      "IF",
      "ELSE",
    },
    {
      // ENUM
      "ELEMENT",
    },
    {
      // TYPE
      "PLAIN",
      "ARRAY",
      "POINTER",
      "FUNCPTR",
    },
    {
      // VAR
      "DECL",
      "DEF",
    },
    {
      // FUNC
      "ARGLIST",
    },
    {
      // EXPR_BINARY_OPERATOR
      "ADD",
      "SUB",
      "MUL",
      "DIV",
      "MOD",
      "SHL",
      "SHR",
      "AND",
      "OR",
      "XOR",
    },
    {
      // EXPR_COMPARE_OPERATOR
      "EQ",
      "NOTEQ",
      "LTHAN",
      "GTHAN",
      "LTHANEQ",
      "GTHANEQ",
    },
    {
      // EXPR_BOOLEAN_OPERATOR
      "AND",
      "OR",
    },
    {
      // EXPR_ASSIGN_OPERATOR
      "EQ",
      "ADDEQ",
      "SUBEQ",
      "MULEQ",
      "DIVEQ",
      "MODEQ",
      "SHLEQ",
      "SHREQ",
      "ANDEQ",
      "OREQ",
      "XOREQ",
    },
    {
      // EXPR_PREFIX_OPERATOR
      "NOT",
      "BNOT",
      "NEG",
      "DEREF",
      "ADDRESS",
    },
    {
      // EXPR_OPERAND
      "IDENTIFIER",
      "NUM",
      "NUMFLOAT",
      "ARRAY_ELEMENT",
      "CAST_TO",
      "CALL",
      "NUMTYPESPEC",
      "STRING",
      "SYMBOL",
      "ACCESS_MEMBER",
      "GENERIC_CALL",
    },
    {
      // VISIBILITY_MARKER
      "PUBLIC",
      "PRIVATE",
    },
    {
      // INITLIST
      "ENTRY",
      "ENTRY_EXPLICIT",
      "ENTRY_EXPLICIT_ARRAY_ELEM",
    },
    {
      // Generic
      "STRUCT",
      "FUNC",
      "PLACEHOLDER_TYPE_LIST",
      "PLACEHOLDER_TYPE",
      "TYPE_LIST",
      "TYPE",
      "NAMESPACE",
    },
  };

  size_t group = __AST_GET_GROUP(type);
  size_t id = __AST_GET_ID(type);

  std::string out = groups[group];
  if (!out.empty()) {
    out += '_';
  }
  out += ids[group][id];

  return out;
}
}
