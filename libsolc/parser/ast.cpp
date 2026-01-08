#include "parser/ast.hpp"
#include <cstdint>

namespace solc
{

static std::string asttype_to_string(ASTType type);

static std::vector<std::string> build_tree(const AST &ast, size_t depth)
{
  std::vector<std::string> out{};
  if (depth == 0) {
    out.emplace_back("[ROOT] " + asttype_to_string(ast.type) + " \"" +
                     ast.value + "\"");
  } else {
    out.emplace_back("(" + std::to_string(depth) + ") " +
                     asttype_to_string(ast.type) + " \"" + ast.value + "\"");
  }

  std::vector<std::vector<std::string> > newvs{};
  for (auto &c : ast.children) {
    newvs.emplace_back(build_tree(c, depth + 1));
  }

  size_t newvs_len = newvs.size();
  for (size_t i = 0; i < newvs_len; i++) {
    auto &newv = newvs.at(i);

    auto newv_len = newv.size();
    for (size_t j = 0; j < newv_len; j++) {
      if (j == 0) {
        if (i == newvs_len - 1) {
          newv[j] = " ╰" + newv[j];
        } else {
          newv[j] = " ├" + newv[j];
        }
      } else if (i < newvs_len - 1) {
        newv[j] = " │" + newv[j];
      } else {
        newv[j] = "  " + newv[j];
      }
    }
  }

  for (auto &newv : newvs) {
    out.insert(out.end(), newv.begin(), newv.end());
  }

  return out;
}

std::string AST::to_string() const
{
  auto vs = build_tree(*this, 0);
  std::string out{};
  for (auto &s : vs) {
    out += s + '\n';
  }
  return out;
}

bool AST::is_empty() const
{
  return children.empty();
}

AST *AST::append(AST child)
{
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
      "EXPORTED_FUNC", "EXTERN_FUNC", "QUALIFIER",   "NONE",    "LABEL",
      "VARIADIC",      "STRUCT",      "UNION",       "ENUM",    "FUNC",
      "NAMESPACE",     "INITLIST",    "PREFIX_EXPR", "GENERIC", "FUNC_EXPLICIT",
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
      "TYPEOF",
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
      "VOID",
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
      "SIZEOF",
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
      "FUNC_EXPLICIT",
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
