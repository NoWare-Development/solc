#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast_func_type.h"
#include "solc/defs.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include "parser/parser_private.h"

solc_ast_t *solc_parser_parse_def_func(solc_parser_t *parser,
                                       solc_ast_func_type_t func_type)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);
  const char *func_name = parser->tokens[parser->pos].value;
  sz func_pos = parser->pos++;

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
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  solc_ast_t *arglist_ast = solc_parser_parse_func_arglist(parser);
  solc_ast_t *type_ast = nullptr;

  const solc_token_t *next = solc_parser_peek_token(parser, parser->pos);
  if (next != nullptr && next->type == SOLC_TOKENTYPE_MINUS &&
      !next->has_whitespace_after &&
      solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_RARROW) {
    parser->pos += 2;
    VERIFY_POS(parser, parser->pos);

    type_ast = solc_parser_parse_type(parser);
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  solc_ast_t *block = solc_parser_parse_stmt_block(parser);

  return solc_ast_func_create(func_pos, func_name, type_ast, arglist_ast, block,
                              func_type);
}

solc_ast_t *solc_parser_parse_def_func_generic(solc_parser_t *parser,
                                               solc_ast_func_type_t func_type)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);
  const char *generic_func_name = parser->tokens[parser->pos].value;
  sz generic_func_pos = parser->pos++;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LARROW);

  solc_ast_t *generic_placeholder_type_list_ast =
    solc_parser_parse_generic_placeholder_type_list(parser);

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
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  solc_ast_t *arglist_ast = solc_parser_parse_func_arglist(parser);
  solc_ast_t *type_ast = nullptr;

  const solc_token_t *next = solc_parser_peek_token(parser, parser->pos);
  if (next != nullptr && next->type == SOLC_TOKENTYPE_MINUS &&
      !next->has_whitespace_after &&
      solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_RARROW) {
    parser->pos += 2;
    VERIFY_POS(parser, parser->pos);

    type_ast = solc_parser_parse_type(parser);
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  solc_ast_t *block = solc_parser_parse_stmt_block(parser);

  return solc_ast_generic_func_create(generic_func_pos, generic_func_name,
                                      type_ast, arglist_ast, block,
                                      generic_placeholder_type_list_ast,
                                      func_type);
}

solc_ast_t *solc_parser_parse_export(solc_parser_t *parser)
{
  parser->pos++;

  return solc_parser_parse_def_func(parser, SOLC_AST_FUNC_TYPE_EXPORTED);
}
