#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_context.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_block(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  solc_ast_t *block = solc_ast_stmt_block_create(parser->pos++);
  VERIFY_POS(parser, parser->pos);

  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RCBRACK)
      break;

    solc_ast_stmt_block_add_stmt(block, solc_parser_parse_stmt(parser));
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);

  parser->pos++;

  return block;
}

solc_ast_t *solc_parser_parse_stmt(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);

  switch (parser->tokens[parser->pos].type) {
  case SOLC_TOKENTYPE_SEMI: {
    return solc_ast_none_create(parser->pos++);
  }

  case SOLC_TOKENTYPE_LCBRACK: {
    return solc_parser_parse_stmt_block(parser);
  }

  case SOLC_TOKENTYPE_ID: {
    parser_stmt_func_t func =
      parser_context_get_stmt_func(parser->tokens[parser->pos].value);

    if (func != nullptr) {
      return func(parser);
    } else if (parser_context_is_qualifier(parser->tokens[parser->pos].value)) {
      return solc_parser_parse_decldef(parser);
    }

    if (solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_COLON) {
      if (!parser->tokens[parser->pos].has_whitespace_after &&
          solc_parser_peek(parser, parser->pos + 2) == SOLC_TOKENTYPE_COLON &&
          solc_parser_peek(parser, parser->pos + 3) != SOLC_TOKENTYPE_LPAREN) {
        break;
      }
      return solc_parser_parse_decldef(parser);
    }
  } break;

  case SOLC_TOKENTYPE_AT: {
    return solc_parser_parse_stmt_label(parser);
  }

  default:
    break;
  }

  solc_ast_t *out =
    solc_parser_parse_stmt_expr_or_generic_func_or_namespace(parser);
  if (parser->errored) {
    if (out != nullptr)
      solc_ast_destroy(out);
    parser->pos++;
    return nullptr;
  }

  return out;
}

solc_ast_t *
solc_parser_parse_stmt_expr_or_generic_func_or_namespace(solc_parser_t *parser)
{
  sz pos = parser->pos;

  VERIFY_POS(parser, parser->pos);
  if (parser->tokens[pos].type == SOLC_TOKENTYPE_ID) {
    pos++;
    if (solc_parser_peek(parser, pos++) == SOLC_TOKENTYPE_LARROW) {
      b8 not_a_generic_func = false;
      while (pos < parser->tokens_num && !not_a_generic_func) {
        switch (parser->tokens[pos].type) {
        case SOLC_TOKENTYPE_ID:
        case SOLC_TOKENTYPE_COMMA:
          break;

        case SOLC_TOKENTYPE_RARROW: {
          const solc_token_t *peeked_1 =
            solc_parser_peek_token(parser, pos + 1);
          solc_tokentype_t peeked_2 = solc_parser_peek(parser, pos + 2);

          if (peeked_1 != nullptr && peeked_1->type == SOLC_TOKENTYPE_COLON &&
              !peeked_1->has_whitespace_after &&
              peeked_2 == SOLC_TOKENTYPE_COLON) {
            switch (solc_parser_peek(parser, pos + 3)) {
            case SOLC_TOKENTYPE_LPAREN:
              return solc_parser_parse_def_func_generic(parser);

            case SOLC_TOKENTYPE_ID: {
              solc_ast_t *generic_namespace =
                solc_parser_parse_generic_namespace(parser);
              solc_ast_t *symbol =
                solc_parser_parse_expr_operand_identifier(parser, true, true);
              solc_ast_generic_namespace_set_subobject(generic_namespace,
                                                       symbol);
              return generic_namespace;
            }

            case SOLC_TOKENTYPE_ERR: {
              solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_EXPECTED,
                                    pos + 3, 1, SOLC_TOKENTYPE_ERR);
              return nullptr;
            }

            default: {
              solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED,
                                    pos + 3, 1, SOLC_TOKENTYPE_ERR);
              return nullptr;
            }
            }
          }

          not_a_generic_func = true;
        } break;

        default:
          not_a_generic_func = true;
          break;
        }

        pos++;
      }
    }
  }

  return solc_parser_parse_stmt_expr(parser);
}
