#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_expr_operand.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/ast_op_types.h"
#include "parser/parser_private.h"
#include "solc/defs.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  union {
    solc_ast_t *ast;
    struct {
      sz operator_pos;
      expr_operator_type_t operator_type;
    };
  };
  b8 is_operator;
} ast_op_union_t;

static inline void get_binding_power(expr_operator_type_t operator, s32 *l_bp,
                                     s32 *r_bp);

static inline ast_op_union_t *parse_expr_data(solc_parser_t *parser);
static inline b8 validate_expr_data(solc_parser_t *parser, sz start_pos,
                                    ast_op_union_t *ast_op_unions_v,
                                    b8 toplevel);
static solc_ast_t *pratt_parse_expr(ast_op_union_t *ast_op_unions_v, sz *pos,
                                    s32 min_bp);
static expr_operator_type_t
token_to_expr_operator(expr_operator_group_t operator_group,
                       solc_tokentype_t type);

static inline u64 num_tok_to_u64(solc_token_t *token);

solc_ast_t *solc_parser_parse_stmt_expr(solc_parser_t *parser)
{
  sz expr_pos = parser->pos;
  solc_ast_t *expr_ast = solc_parser_parse_expr(parser, true);
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_SEMI);
  parser->pos++;

  solc_ast_t *expr_stmt_ast = solc_ast_stmt_expr_create(expr_pos, expr_ast);
  return expr_stmt_ast;
}

solc_ast_t *solc_parser_parse_expr(solc_parser_t *parser, b8 toplevel)
{
  VERIFY_POS(parser, parser->pos);
  sz start_pos = parser->pos;
  ast_op_union_t *ast_op_unions_v = parse_expr_data(parser);
  solc_ast_t *out = nullptr;
  out = !validate_expr_data(parser, start_pos, ast_op_unions_v, toplevel) ?
          nullptr :
          pratt_parse_expr(ast_op_unions_v, nullptr, 0);

  if (out == nullptr)
    for (sz i = 0, ast_op_unions_v_size = vector_get_length(ast_op_unions_v);
         i < ast_op_unions_v_size; i++)
      if (!ast_op_unions_v[i].is_operator)
        solc_ast_destroy(ast_op_unions_v[i].ast);
  vector_destroy(ast_op_unions_v);

  return out;
}

static inline ast_op_union_t *parse_expr_data(solc_parser_t *parser)
{
  ast_op_union_t *out_ast_op_unions_v = vector_create(ast_op_union_t);

  typedef struct {
    union {
      solc_ast_type_t ast_type;
      expr_operator_type_t operator_type;
    };
    b8 is_operator;
  } ast_type_op_union_t;

  ast_type_op_union_t prev = {
    .ast_type = SOLC_AST_TYPE_NONE_ERR,
    .is_operator = false,
  };
  while (parser->pos < parser->tokens_num) {
    solc_token_t cur_tok = parser->tokens[parser->pos];

    // Prefix operators
    if ((prev.is_operator || prev.ast_type == SOLC_AST_TYPE_NONE_ERR) &&
        solc_parser_is_prefix_operator_token(cur_tok.type)) {
      ast_op_union_t out_op = {
        .operator_pos = parser->pos,
        .operator_type =
          token_to_expr_operator(EXPR_OPERATOR_GROUP_PREFIX, cur_tok.type),
        .is_operator = true,
      };
      vector_push(out_ast_op_unions_v, out_op);
      parser->pos++;

      prev.is_operator = true;
      prev.operator_type = out_op.operator_type;
      continue;
    }

    // Operators
    else if (!prev.is_operator && prev.ast_type != SOLC_AST_TYPE_NONE_ERR) {
      if (!cur_tok.has_whitespace_after) {
        // == != >= <=
        // += -= *= /= %= &= |= ^=
        if (solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_EQ) {
          switch (cur_tok.type) {
          // Compare operators (except < and >)
          case SOLC_TOKENTYPE_EXCLMARK: { // !=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_COMPARE_NOTEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_EQ: { // ==
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_COMPARE_EQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_RARROW: { // >=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_COMPARE_GTHANEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_LARROW: { // <=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_COMPARE_LTHANEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }

          // Assign operators (except =, >>= and <<=)
          case SOLC_TOKENTYPE_PLUS: { // +=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_ADDEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_MINUS: { // -=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_SUBEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_ASTERISK: { // *=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_MULEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_SLASH: { // /=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_DIVEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_PERCENT: { // %=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_MODEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_AMPERSAND: { // &=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_ANDEQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_PIPE: { // |=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_OREQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_CIRCUMFLEX: { // ^=
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_XOREQ,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }

          default: // Neither of those
            break;
          }
        }

        const solc_token_t *peeked_token =
          solc_parser_peek_token(parser, parser->pos + 1);
        if (peeked_token != nullptr && peeked_token->type == cur_tok.type) {
          if (solc_parser_peek(parser, parser->pos + 2) == SOLC_TOKENTYPE_EQ &&
              !peeked_token->has_whitespace_after) {
            // >>= <<=
            switch (cur_tok.type) {
            case SOLC_TOKENTYPE_RARROW: { // >>=
              ast_op_union_t out_op = {
                .operator_pos = parser->pos,
                .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_SHREQ,
                .is_operator = true,
              };
              vector_push(out_ast_op_unions_v, out_op);
              parser->pos += 3;
              prev.is_operator = true;
              prev.operator_type = out_op.operator_type;
              continue;
            }
            case SOLC_TOKENTYPE_LARROW: { // <<=
              ast_op_union_t out_op = {
                .operator_pos = parser->pos,
                .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_SHLEQ,
                .is_operator = true,
              };
              vector_push(out_ast_op_unions_v, out_op);
              parser->pos += 3;
              prev.is_operator = true;
              prev.operator_type = out_op.operator_type;
              continue;
            }

            default:
              break;
            }
          }

          // && || << >>
          switch (cur_tok.type) {
          // Boolean operators (&&, ||)
          case SOLC_TOKENTYPE_AMPERSAND: { // &&
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_BOOLEAN_AND,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_PIPE: { // ||
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_BOOLEAN_OR,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }

          // Binary operators (<<, >>)
          case SOLC_TOKENTYPE_LARROW: { // <<
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_BINARY_SHL,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }
          case SOLC_TOKENTYPE_RARROW: { // >>
            ast_op_union_t out_op = {
              .operator_pos = parser->pos,
              .operator_type = EXPR_OPERATOR_TYPE_BINARY_SHR,
              .is_operator = true,
            };
            vector_push(out_ast_op_unions_v, out_op);
            parser->pos += 2;
            prev.is_operator = true;
            prev.operator_type = out_op.operator_type;
            continue;
          }

          default:
            break;
          }
        }
      }

      switch (cur_tok.type) {
      // Remaining compare operators (< >)
      case SOLC_TOKENTYPE_LARROW: { // <
        ast_op_union_t out_op = {
          .operator_pos = parser->pos,
          .operator_type = EXPR_OPERATOR_TYPE_COMPARE_LTHAN,
          .is_operator = true,
        };
        vector_push(out_ast_op_unions_v, out_op);
        parser->pos++;
        prev.is_operator = true;
        prev.operator_type = out_op.operator_type;
        continue;
      }
      case SOLC_TOKENTYPE_RARROW: { // >
        ast_op_union_t out_op = {
          .operator_pos = parser->pos,
          .operator_type = EXPR_OPERATOR_TYPE_COMPARE_GTHAN,
          .is_operator = true,
        };
        vector_push(out_ast_op_unions_v, out_op);
        parser->pos++;
        prev.is_operator = true;
        prev.operator_type = out_op.operator_type;
        continue;
      }

      // =
      case SOLC_TOKENTYPE_EQ: {
        ast_op_union_t out_op = {
          .operator_pos = parser->pos,
          .operator_type = EXPR_OPERATOR_TYPE_ASSIGN_EQ,
          .is_operator = true,
        };
        vector_push(out_ast_op_unions_v, out_op);
        parser->pos++;
        prev.is_operator = true;
        prev.operator_type = out_op.operator_type;
        continue;
      }

      default:
        break;
      }

      // Remaining binary operators
      if (solc_parser_is_binary_operator_token(cur_tok.type)) {
        ast_op_union_t out_op = {
          .operator_pos = parser->pos,
          .operator_type =
            token_to_expr_operator(EXPR_OPERATOR_GROUP_BINARY, cur_tok.type),
          .is_operator = true,
        };
        vector_push(out_ast_op_unions_v, out_op);
        parser->pos++;
        prev.is_operator = true;
        prev.operator_type = out_op.operator_type;
        continue;
      }
    }

    else if (prev.is_operator || prev.ast_type == SOLC_AST_TYPE_NONE_ERR) {
      switch (cur_tok.type) {
      default:
        if (!solc_parser_is_numeric_token(cur_tok.type))
          break;
        // Numeric token

        /* fallthrough */

      case SOLC_TOKENTYPE_ID: // Identifier
      case SOLC_TOKENTYPE_LPAREN: // Nested expression
      case SOLC_TOKENTYPE_STRING: // String
      case SOLC_TOKENTYPE_SYMBOL: // Symbol
      {
        ast_op_union_t out_operand = {
          .ast = solc_parser_parse_expr_operand(parser),
          .is_operator = false,
        };
        vector_push(out_ast_op_unions_v, out_operand);
        prev.is_operator = false;
        prev.ast_type = out_operand.ast != nullptr ? out_operand.ast->type :
                                                     SOLC_AST_TYPE_NONE_ERR;
        continue;
      }
      }
    }

    break;
  }

  return out_ast_op_unions_v;
}

solc_ast_t *solc_parser_parse_expr_operand(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  solc_token_t cur_tok = parser->tokens[parser->pos];

  solc_ast_t *out_operand = nullptr;

  switch (cur_tok.type) {
  case SOLC_TOKENTYPE_LPAREN: {
    parser->pos++;
    VERIFY_POS(parser, parser->pos);

    solc_ast_t *nested_expr = solc_parser_parse_expr(parser, false);
    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_RPAREN);

    parser->pos++;

    return nested_expr;
  }

  case SOLC_TOKENTYPE_ID: {
    if (strcmp(cur_tok.value, "cast") == 0) {
      sz cast_pos = parser->pos;
      parser->pos++;

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_LPAREN);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
      solc_ast_t *type_ast = solc_parser_parse_type(parser);

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_RPAREN);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
      solc_ast_t *cast_expr = solc_parser_parse_expr_operand(parser);

      return solc_ast_expr_operand_cast_to_create(cast_pos, type_ast,
                                                  cast_expr);
    } else if (strcmp(cur_tok.value, "sizeof") == 0) {
      sz sizeof_pos = parser->pos++;

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_LPAREN);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
      solc_ast_t *type_ast = solc_parser_parse_type(parser);

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_RPAREN);

      parser->pos++;

      return solc_ast_expr_operand_sizeof_create(sizeof_pos, type_ast);
    } else if (strcmp(cur_tok.value, "alignof") == 0) {
      sz alignof_pos = parser->pos++;

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_LPAREN);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
      solc_ast_t *expr_ast = solc_parser_parse_expr(parser, false);

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_RPAREN);

      parser->pos++;

      return solc_ast_expr_operand_alignof_create(alignof_pos, expr_ast);
    }

    out_operand = solc_parser_parse_expr_operand_identifier(parser, true, true);
    break;
  }

  case SOLC_TOKENTYPE_NUM:
  case SOLC_TOKENTYPE_NUMHEX:
  case SOLC_TOKENTYPE_NUMBIN:
  case SOLC_TOKENTYPE_NUMOCT: {
    sz num_pos = parser->pos;
    uint64_t value = num_tok_to_u64(&cur_tok);
    parser->pos++;
    const char *typespec = nullptr;
    if (parser->pos < parser->tokens_num &&
        parser->tokens[parser->pos].type == SOLC_TOKENTYPE_ID)
      typespec = parser->tokens[parser->pos++].value;
    return solc_ast_expr_operand_num_create(num_pos, value, typespec);
  }

  case SOLC_TOKENTYPE_NUMFLOAT: {
    sz num_pos = parser->pos;
    f64 value = strtod(cur_tok.value, nullptr);
    parser->pos++;
    const char *typespec = nullptr;
    if (parser->pos < parser->tokens_num &&
        parser->tokens[parser->pos].type == SOLC_TOKENTYPE_ID)
      typespec = parser->tokens[parser->pos++].value;

    return solc_ast_expr_operand_numfloat_create(num_pos, value, typespec);
  }

  case SOLC_TOKENTYPE_STRING: {
    sz string_pos = parser->pos;
    string_t out_string = string_create_from(cur_tok.value);
    parser->pos++;
    while (parser->pos < parser->tokens_num) {
      if (parser->tokens[parser->pos].type != SOLC_TOKENTYPE_STRING)
        break;

      string_append_cstr(&out_string, parser->tokens[parser->pos].value);
      parser->pos++;
    }
    solc_ast_t *out_string_ast =
      solc_ast_expr_operand_string_create(string_pos, out_string.data);
    string_destroy(&out_string);
    return out_string_ast;
  }

  case SOLC_TOKENTYPE_SYMBOL: {
    sz symbol_pos = parser->pos++;
    char c;
    if (cur_tok.value[0] == '\\') {
      switch (cur_tok.value[1]) {
      case '\\':
        c = '\\';
        break;
      case '\"':
        c = '\"';
        break;
      case '\'':
        c = '\'';
        break;
      case 'n':
        c = '\n';
        break;
      case 't':
        c = '\t';
        break;
      case 'r':
        c = '\r';
        break;

      case 0: {
        solc_parser_add_error(parser,
                              SOLC_PARSER_ERROR_TYPE_NO_CHAR_AFTER_BACKSLASH,
                              symbol_pos, 1, SOLC_TOKENTYPE_ERR);
        return nullptr;
      }
      default: {
        solc_parser_add_error(parser,
                              SOLC_PARSER_ERROR_TYPE_UNKNOWN_SPECIAL_CHARACTER,
                              symbol_pos, 1, SOLC_TOKENTYPE_ERR);
        return nullptr;
      }
      }
    } else {
      c = cur_tok.value[0];
    }

    return solc_ast_expr_operand_symbol_create(symbol_pos, c);
  }

  default:
    solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED,
                          parser->pos++, 1, SOLC_TOKENTYPE_ERR);
    return nullptr;
  }

  out_operand =
    solc_parser_parse_expr_operand_array_element(parser, out_operand);

  if (solc_parser_peek(parser, parser->pos) == SOLC_TOKENTYPE_PERIOD) {
    sz access_pos = parser->pos++;
    VERIFY_POS(parser, parser->pos);
    solc_ast_t *symbol = solc_parser_parse_expr_operand(parser);

    solc_ast_t *access_ast = solc_ast_expr_operand_access_member_create(
      access_pos, out_operand, symbol);
    out_operand = access_ast;
  }

  return out_operand;
}

solc_ast_t *solc_parser_parse_expr_operand_identifier(solc_parser_t *parser,
                                                      b8 accept_namespaces,
                                                      b8 accept_functions)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const solc_token_t *next = solc_parser_peek_token(parser, parser->pos + 1);
  if (accept_namespaces) {
    if (next != nullptr && next->type == SOLC_TOKENTYPE_COLON &&
        !next->has_whitespace_after &&
        solc_parser_peek(parser, parser->pos + 2) == SOLC_TOKENTYPE_COLON) {
      sz namespace_pos = parser->pos;
      const char *namespace_name = parser->tokens[parser->pos].value;

      parser->pos += 3;
      solc_ast_t *symbol = solc_parser_parse_expr_operand_identifier(
        parser, accept_namespaces, accept_functions);

      return solc_ast_namespace_create(namespace_pos, namespace_name, symbol);
    } else if (solc_parser_is_generic_namespace(parser)) {
      sz generic_namespace_pos = parser->pos;
      const char *generic_namespace_name = parser->tokens[parser->pos].value;

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_LARROW);

      solc_ast_t *generic_type_list =
        solc_parser_parse_generic_type_list(parser);

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_COLON);
      VERIFY_WHITESPACE(parser, parser->pos,
                        parser->tokens[parser->pos].has_whitespace_after, false,
                        SOLC_TOKENTYPE_COLON);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_COLON);

      parser->pos++;

      solc_ast_t *symbol = solc_parser_parse_expr_operand_identifier(
        parser, accept_namespaces, accept_functions);

      return solc_ast_generic_namespace_create(generic_namespace_pos,
                                               generic_namespace_name,
                                               generic_type_list, symbol);
    }
  }

  solc_ast_t *out_operand = nullptr;

  if (accept_functions) {
    if (next != nullptr && next->type == SOLC_TOKENTYPE_LPAREN) {
      out_operand = solc_parser_parse_expr_operand_call(parser);
      goto idop_process_out_operand;
    } else if (solc_parser_is_expr_operand_generic_call(parser)) {
      out_operand = solc_parser_parse_expr_operand_generic_call(parser);
      goto idop_process_out_operand;
    }
  }

  out_operand = solc_ast_expr_operand_identifier_create(
    parser->pos, parser->tokens[parser->pos].value);
  parser->pos++;

idop_process_out_operand:
  if (solc_parser_peek(parser, parser->pos) == SOLC_TOKENTYPE_PERIOD) {
    sz access_pos = parser->pos++;

    VERIFY_POS(parser, parser->pos);
    solc_ast_t *object = solc_parser_parse_expr_operand_identifier(
      parser, false, accept_functions);

    return solc_ast_expr_operand_access_member_create(access_pos, out_operand,
                                                      object);
  }

  return out_operand;
}

solc_ast_t *solc_parser_parse_expr_operand_call(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  solc_ast_t *call_operand = solc_ast_expr_operand_call_create(
    parser->pos, parser->tokens[parser->pos].value);
  parser->pos++;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  while (parser->pos < parser->tokens_num) {
    switch (parser->tokens[parser->pos].type) {
    case SOLC_TOKENTYPE_RPAREN:
      goto expr_operand_call_after_loop;

    case SOLC_TOKENTYPE_LCBRACK: {
      solc_ast_t *initlist = solc_parser_parse_initlist(parser);
      solc_ast_expr_operand_call_add_argument(call_operand, initlist);
      break;
    }

    default: {
      solc_ast_t *expr = solc_parser_parse_expr(parser, false);
      solc_ast_expr_operand_call_add_argument(call_operand, expr);
      break;
    }
    }

    VERIFY_POS(parser, parser->pos);
    solc_tokentype_t next = parser->tokens[parser->pos].type;
    if (next != SOLC_TOKENTYPE_RPAREN) {
      VERIFY_TOKEN(parser, parser->pos, next, SOLC_TOKENTYPE_COMMA);
      parser->pos++;
    }
  }

expr_operand_call_after_loop:
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RPAREN);
  parser->pos++;

  return call_operand;
}

solc_ast_t *solc_parser_parse_expr_operand_generic_call(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  sz generic_call_operand_pos = parser->pos;
  const char *generic_call_operand_callee_name =
    parser->tokens[parser->pos].value;

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LARROW);

  solc_ast_t *generic_type_list = solc_parser_parse_generic_type_list(parser);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  solc_ast_t *generic_call_operand = solc_ast_expr_operand_generic_call_create(
    generic_call_operand_pos, generic_call_operand_callee_name,
    generic_type_list);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  while (parser->pos < parser->tokens_num) {
    switch (parser->tokens[parser->pos].type) {
    case SOLC_TOKENTYPE_RPAREN:
      goto expr_operand_generic_call_after_loop;

    case SOLC_TOKENTYPE_LCBRACK: {
      solc_ast_t *initlist = solc_parser_parse_initlist(parser);
      solc_ast_expr_operand_generic_call_add_argument(generic_call_operand,
                                                      initlist);
      break;
    }

    default: {
      solc_ast_t *expr = solc_parser_parse_expr(parser, false);
      solc_ast_expr_operand_generic_call_add_argument(generic_call_operand,
                                                      expr);
      break;
    }
    }

    VERIFY_POS(parser, parser->pos);
    solc_tokentype_t next = parser->tokens[parser->pos].type;
    if (next != SOLC_TOKENTYPE_RPAREN) {
      VERIFY_TOKEN(parser, parser->pos, next, SOLC_TOKENTYPE_COMMA);
      parser->pos++;
    }
  }

expr_operand_generic_call_after_loop:
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RPAREN);
  parser->pos++;

  return generic_call_operand;
}

static inline b8 validate_expr_data(solc_parser_t *parser, sz start_pos,
                                    ast_op_union_t *ast_op_unions_v,
                                    b8 toplevel)
{
  sz ast_op_unions_v_size = vector_get_length(ast_op_unions_v);
  if SOLC_UNLIKELY (ast_op_unions_v_size == 0) {
    solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_EXPR_EMPTY, start_pos,
                          1, SOLC_TOKENTYPE_ERR);
    return false;
  }

  ast_op_union_t *prev = nullptr, *cur = nullptr;
  for (sz i = 0; i < ast_op_unions_v_size; i++, prev = cur) {
    cur = &ast_op_unions_v[i];
    sz cur_pos = cur->is_operator ? cur->operator_pos : cur->ast->token_pos;

    if ((cur->is_operator && expr_operator_type_get_group(cur->operator_type) ==
                               EXPR_OPERATOR_GROUP_PREFIX) &&
        (prev == nullptr || prev->is_operator ||
         prev->ast->type == SOLC_AST_TYPE_NONE_ERR)) {
      if SOLC_UNLIKELY (i + 1 >= ast_op_unions_v_size) {
        solc_parser_add_error(
          parser, SOLC_PARSER_ERROR_TYPE_EXPR_NO_OPERAND_AFTER_PREFIX, cur_pos,
          1, SOLC_TOKENTYPE_ERR);
        return false;
      }

      ast_op_union_t *next = &ast_op_unions_v[i + 1];
      if SOLC_LIKELY (!next->is_operator ||
                      expr_operator_type_get_group(next->operator_type) ==
                        EXPR_OPERATOR_GROUP_PREFIX)
        continue;

      solc_parser_add_error(parser,
                            SOLC_PARSER_ERROR_TYPE_EXPR_PREFIX_AFTER_OPERAND,
                            cur_pos, 1, SOLC_TOKENTYPE_ERR);
      return false;
    } else if (cur->is_operator) {
      if SOLC_UNLIKELY (expr_operator_type_get_group(cur->operator_type) ==
                          EXPR_OPERATOR_GROUP_ASSIGN &&
                        !toplevel) {
        // TODO: use proper operator length, it can be 1, 2, or 3
        // (depending on an operator).
        solc_parser_add_error(
          parser, SOLC_PARSER_ERROR_TYPE_EXPR_ASSIGN_OPERATOR_IN_NON_TOPLEVEL,
          cur_pos, 1, SOLC_TOKENTYPE_ERR);
        return false;
      } else if SOLC_UNLIKELY (prev == nullptr) {
        solc_parser_add_error(
          parser,
          SOLC_PARSER_ERROR_TYPE_EXPR_NON_PREFIX_OPERATOR_AT_THE_BEGINNING,
          cur_pos, 1, SOLC_TOKENTYPE_ERR);
        return false;
      } else if SOLC_UNLIKELY (prev->is_operator) {
        solc_parser_add_error(
          parser, SOLC_PARSER_ERROR_TYPE_EXPR_2_NON_PREFIX_OPERATORS, cur_pos,
          1, SOLC_TOKENTYPE_ERR);
        return false;
      }
      continue;
    } else if SOLC_UNLIKELY (!cur->is_operator && prev != nullptr &&
                             !prev->is_operator) {
      solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_EXPR_2_OPERANDS,
                            cur_pos, 1, SOLC_TOKENTYPE_ERR);
      return false;
    }
  }

  b8 has_assign_operator = false;
  for (sz i = 0; i < ast_op_unions_v_size; i++) {
    ast_op_union_t *ast_op_union = &ast_op_unions_v[i];
    if (ast_op_union->is_operator &&
        expr_operator_type_get_group(ast_op_union->operator_type) ==
          EXPR_OPERATOR_GROUP_ASSIGN) {
      if SOLC_UNLIKELY (has_assign_operator) {
        solc_parser_add_error(parser,
                              SOLC_PARSER_ERROR_TYPE_EXPR_2_ASSIGN_OPERATORS,
                              ast_op_union->operator_pos, 1,
                              SOLC_TOKENTYPE_ERR);
        return false;
      }
      has_assign_operator = true;
    }
  }

  if SOLC_UNLIKELY (ast_op_unions_v[ast_op_unions_v_size - 1].is_operator) {
    solc_parser_add_error(
      parser, SOLC_PARSER_ERROR_TYPE_EXPR_LAST_NODE_IS_NOT_AN_OPERAND,
      ast_op_unions_v[ast_op_unions_v_size - 1].operator_pos, 1,
      SOLC_TOKENTYPE_ERR);
    return false;
  }

  return true;
}

static solc_ast_t *pratt_parse_expr(ast_op_union_t *ast_op_unions_v, sz *pos,
                                    s32 min_bp)
{
  sz ast_op_unions_v_size = vector_get_length(ast_op_unions_v);

  b8 pos_was_null = false;
  if SOLC_UNLIKELY (pos == nullptr) {
    pos = calloc(1, sizeof(sz));
    pos_was_null = true;
  } else if (*pos >= ast_op_unions_v_size)
    return nullptr;

  solc_ast_t *lhs = nullptr;

  // Handle prefix operators
  if (ast_op_unions_v[*pos].is_operator &&
      expr_operator_type_get_group(ast_op_unions_v[*pos].operator_type) ==
        EXPR_OPERATOR_GROUP_PREFIX) {
    ast_op_union_t *cur = &ast_op_unions_v[(*pos)++];
    expr_operator_type_t *prefix_operators_v =
      vector_create(expr_operator_type_t);

    sz prefix_start = cur->operator_pos;

    // Obtain all prefix operators
    while (cur->is_operator &&
           expr_operator_type_get_group(cur->operator_type) ==
             EXPR_OPERATOR_GROUP_PREFIX &&
           (*pos) < ast_op_unions_v_size) {
      vector_push(prefix_operators_v, cur->operator_type);
      cur = &ast_op_unions_v[(*pos)++];
    }
    solc_ast_t *operand = cur->ast; // Obtain operand

    // Reverse prefix operators list
    for (sz i = 0, j = vector_get_length(prefix_operators_v) - 1; i < j;
         i++, j--) {
      expr_operator_type_t tmp = prefix_operators_v[i];
      prefix_operators_v[i] = prefix_operators_v[j];
      prefix_operators_v[j] = tmp;
    }

    // Create LHS
    lhs =
      solc_ast_prefix_expr_create(prefix_start, operand, prefix_operators_v);
  } else {
    lhs = ast_op_unions_v[(*pos)++].ast;
  }

  while (*pos < ast_op_unions_v_size) {
    expr_operator_type_t op = ast_op_unions_v[*pos].operator_type;

    s32 l_bp, r_bp;
    get_binding_power(op, &l_bp, &r_bp);
    if (l_bp < min_bp)
      break;

    (*pos)++;
    solc_ast_t *rhs = pratt_parse_expr(ast_op_unions_v, pos, r_bp);
    solc_ast_t *new_lhs = solc_ast_expr_create(lhs->token_pos, lhs, rhs, op);
    lhs = new_lhs;
  }

  if SOLC_UNLIKELY (pos_was_null)
    free(pos);

  return lhs;
}

static expr_operator_type_t
token_to_expr_operator(expr_operator_group_t op_group, solc_tokentype_t type)
{
  switch (op_group) {
  case EXPR_OPERATOR_GROUP_BINARY: {
    switch (type) {
    case SOLC_TOKENTYPE_PLUS:
      return EXPR_OPERATOR_TYPE_BINARY_ADD;
    case SOLC_TOKENTYPE_MINUS:
      return EXPR_OPERATOR_TYPE_BINARY_SUB;
    case SOLC_TOKENTYPE_ASTERISK:
      return EXPR_OPERATOR_TYPE_BINARY_MUL;
    case SOLC_TOKENTYPE_SLASH:
      return EXPR_OPERATOR_TYPE_BINARY_DIV;
    case SOLC_TOKENTYPE_PERCENT:
      return EXPR_OPERATOR_TYPE_BINARY_MOD;
    case SOLC_TOKENTYPE_AMPERSAND:
      return EXPR_OPERATOR_TYPE_BINARY_AND;
    case SOLC_TOKENTYPE_PIPE:
      return EXPR_OPERATOR_TYPE_BINARY_OR;
    case SOLC_TOKENTYPE_CIRCUMFLEX:
      return EXPR_OPERATOR_TYPE_BINARY_XOR;

    default:
      SOLC_NOREACH();
    }
  }

  case EXPR_OPERATOR_GROUP_PREFIX: {
    switch (type) {
    case SOLC_TOKENTYPE_ASTERISK:
      return EXPR_OPERATOR_TYPE_PREFIX_DEREF;
    case SOLC_TOKENTYPE_EXCLMARK:
      return EXPR_OPERATOR_TYPE_PREFIX_NOT;
    case SOLC_TOKENTYPE_AMPERSAND:
      return EXPR_OPERATOR_TYPE_PREFIX_ADDRESS;
    case SOLC_TOKENTYPE_MINUS:
      return EXPR_OPERATOR_TYPE_PREFIX_NEG;
    case SOLC_TOKENTYPE_TILDE:
      return EXPR_OPERATOR_TYPE_PREFIX_BNOT;

    default:
      SOLC_NOREACH();
    }
  }

  default:
    SOLC_NOREACH();
  }
}

static inline void get_binding_power(expr_operator_type_t op, s32 *l_bp,
                                     s32 *r_bp)
{
  switch (expr_operator_type_get_group(op)) {
  case EXPR_OPERATOR_GROUP_ASSIGN:
    *l_bp = 10;
    *r_bp = 15;
    return;

  case EXPR_OPERATOR_GROUP_BOOLEAN:
    *l_bp = 60;
    *r_bp = 65;
    return;

  case EXPR_OPERATOR_GROUP_COMPARE:
    *l_bp = 70;
    *r_bp = 75;
    return;

  default:
    break;
  }

  switch (op) {
  case EXPR_OPERATOR_TYPE_BINARY_ADD:
  case EXPR_OPERATOR_TYPE_BINARY_SUB: {
    *l_bp = 20;
    *r_bp = 25;
    return;
  }

  case EXPR_OPERATOR_TYPE_BINARY_MUL:
  case EXPR_OPERATOR_TYPE_BINARY_DIV:
  case EXPR_OPERATOR_TYPE_BINARY_MOD: {
    *l_bp = 30;
    *r_bp = 35;
    return;
  }

  case EXPR_OPERATOR_TYPE_BINARY_SHL:
  case EXPR_OPERATOR_TYPE_BINARY_SHR: {
    *l_bp = 40;
    *r_bp = 45;
    return;
  }

  case EXPR_OPERATOR_TYPE_BINARY_AND:
  case EXPR_OPERATOR_TYPE_BINARY_OR:
  case EXPR_OPERATOR_TYPE_BINARY_XOR: {
    *l_bp = 50;
    *r_bp = 55;
    return;
  }

  default:
    SOLC_NOREACH();
  }
}

static inline u64 num_tok_to_u64(solc_token_t *token)
{
  u64 out = 0;

  const char *s = token->value;

  switch (token->type) {
  case SOLC_TOKENTYPE_NUM: {
    for (; *s; s++) {
      out *= 10;
      out += *s - '0';
    }
  } break;

  case SOLC_TOKENTYPE_NUMBIN: {
    for (; *s; s++) {
      out <<= 1;
      out |= (*s - '0') & 1;
    }
  } break;

  case SOLC_TOKENTYPE_NUMOCT: {
    for (; *s; s++) {
      out <<= 3;
      out += *s - '0';
    }
  } break;

  case SOLC_TOKENTYPE_NUMHEX: {
    for (; *s; s++) {
      u64 x;
      if (*s >= 'a' && *s <= 'f')
        x = 10 + *s - 'a';
      else if (*s >= 'A' && *s <= 'F')
        x = 10 + *s - 'A';
      else
        x = *s - '0';
      out <<= 4;
      out += x;
    }
  } break;

  default:
    SOLC_NOREACH();
  }

  return out;
}
