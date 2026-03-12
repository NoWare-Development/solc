#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_var.h"
#include "parser/ast_func_type.h"
#include "parser/parser_private.h"
#include "solc/defs.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include <string.h>

solc_ast_t *solc_parser_parse_decldef(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);

  solc_token_t cur = parser->tokens[parser->pos];
  if (solc_parser_is_qualifier(cur.value)) {
    sz pos = parser->pos++;
    return solc_ast_qualifier_create(pos, cur.value,
                                     solc_parser_parse_decldef(parser));
  }

  if (cur.type == SOLC_TOKENTYPE_ID && strcmp(cur.value, "func") == 0) {
    parser->pos++;
    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_ID);

    VERIFY_POS(parser, parser->pos + 1);
    if (parser->tokens[parser->pos + 1].type == SOLC_TOKENTYPE_LARROW) {
      return solc_parser_parse_def_func_generic(parser,
                                                SOLC_AST_FUNC_TYPE_EXPLICIT);
    }

    return solc_parser_parse_def_func(parser, SOLC_AST_FUNC_TYPE_EXPLICIT);
  }

  VERIFY_POS(parser, parser->pos + 1);
  switch (parser->tokens[parser->pos + 1].type) {
  case SOLC_TOKENTYPE_COLON: {
    if (!parser->tokens[parser->pos + 1].has_whitespace_after &&
        solc_parser_peek(parser, parser->pos + 2) == SOLC_TOKENTYPE_COLON) {
      return solc_parser_parse_def_func(parser, SOLC_AST_FUNC_TYPE_DEFAULT);
    }

    solc_ast_t *var_decldef_ast = solc_parser_parse_decldef_var(parser);
    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_SEMI);
    parser->pos++;
    return var_decldef_ast;
  }

  case SOLC_TOKENTYPE_LARROW: {
    return solc_parser_parse_def_func_generic(parser,
                                              SOLC_AST_FUNC_TYPE_DEFAULT);
  }

  default:
    break;
  }

  solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED, parser->pos,
                        1, SOLC_TOKENTYPE_ERR);

  return nullptr;
}

solc_ast_t *solc_parser_parse_decldef_var(solc_parser_t *parser)
{
  sz start_pos = parser->pos;

  const char *id_value = parser->tokens[parser->pos++].value;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_COLON);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *type_ast = nullptr;
  if (!parser->tokens[parser->pos - 1].has_whitespace_after &&
      parser->tokens[parser->pos].type == SOLC_TOKENTYPE_EQ) {
    goto parse_var_def;
  }

  type_ast = solc_parser_parse_type(parser);

  if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_EQ) {
parse_var_def:
    parser->pos++;
    VERIFY_POS(parser, parser->pos);

    solc_ast_t *expr = parser->tokens[parser->pos].type ==
                           SOLC_TOKENTYPE_LCBRACK ?
                         solc_parser_parse_initlist(parser) :
                         solc_parser_parse_expr(parser, false);
    return solc_ast_var_def_create(start_pos, id_value, type_ast, expr);
  }

  return solc_ast_var_decl_create(start_pos, id_value, type_ast);
}
