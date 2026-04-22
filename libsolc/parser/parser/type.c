#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_type.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/parser.h"
#include "solc/parser/ast.h"
#include <string.h>

solc_ast_t *solc_parser_parse_type(solc_parser_t *parser)
{
  solc_ast_t *type_raw = solc_parser_parse_type_raw(parser);

  // Parse all nested arrays which may be after raw type
  solc_tokentype_t next = solc_parser_peek(parser, parser->pos);
  solc_ast_t *current = type_raw;
  while (parser->pos < parser->tokens_num && next == SOLC_TOKENTYPE_LBRACK) {
    sz array_pos = parser->pos++;
    solc_ast_t *expr = nullptr;

    next = solc_parser_peek(parser, parser->pos);
    if (next != SOLC_TOKENTYPE_RBRACK) {
      expr = solc_parser_parse_expr(parser, false);
      VERIFY_POS(parser, parser->pos);
      next = solc_parser_peek(parser, parser->pos);
    }

    VERIFY_TOKEN(parser, parser->pos, next, SOLC_TOKENTYPE_RBRACK);
    parser->pos++;

    current = solc_ast_type_array_create(array_pos, expr, current);
    next = solc_parser_peek(parser, parser->pos);
  }

  return current;
}

solc_ast_t *solc_parser_parse_type_raw(solc_parser_t *parser)
{
  sz ptr_num;
  sz ptr_start = parser->pos;
  for (ptr_num = 0; parser->pos < parser->tokens_num &&
                    parser->tokens[parser->pos].type == SOLC_TOKENTYPE_ASTERISK;
       parser->pos++, ptr_num++)
    ;

  VERIFY_POS(parser, parser->pos);

  solc_ast_t *type;
  solc_token_t cur = parser->tokens[parser->pos];
  if (cur.type == SOLC_TOKENTYPE_LPAREN) {
    type = solc_parser_parse_type_funcptr(parser);
    goto process_ptrs;
  }

  VERIFY_TOKEN(parser, parser->pos, cur.type, SOLC_TOKENTYPE_ID);
  if (strcmp(cur.value, "typeof") == 0) {
    sz typeof_pos = parser->pos++;

    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_LPAREN);

    parser->pos++;
    VERIFY_POS(parser, parser->pos);
    solc_ast_t *expr = solc_parser_parse_expr(parser, false);

    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_RPAREN);

    parser->pos++;
    type = solc_ast_type_typeof_create(typeof_pos, expr);
    goto process_ptrs;
  }

  const solc_token_t *next = solc_parser_peek_token(parser, parser->pos + 1);
  if (next != nullptr && next->type == SOLC_TOKENTYPE_COLON &&
      !next->has_whitespace_after &&
      solc_parser_peek(parser, parser->pos + 2) == SOLC_TOKENTYPE_COLON) {
    sz namespace_pos = parser->pos;
    parser->pos += 3;
    VERIFY_POS(parser, parser->pos);
    solc_ast_t *member_type = solc_parser_parse_type_raw(parser);
    type = solc_ast_namespace_create(namespace_pos, cur.value, member_type);
    goto process_ptrs;
  }

  sz plain_type_pos = parser->pos++;
  if (solc_parser_peek(parser, parser->pos) == SOLC_TOKENTYPE_EXCLMARK) {
    solc_ast_t *generic_type_list = solc_parser_parse_generic_type_list(parser);

    if (parser->pos + 1 < parser->tokens_num &&
        parser->tokens[parser->pos].type == SOLC_TOKENTYPE_COLON &&
        parser->tokens[parser->pos + 1].type == SOLC_TOKENTYPE_COLON &&
        !parser->tokens[parser->pos].has_whitespace_after) {
      parser->pos += 2;
      solc_ast_t *member_type = solc_parser_parse_type_raw(parser);
      type = solc_ast_generic_namespace_create(plain_type_pos, cur.value,
                                               generic_type_list, member_type);
    } else
      type = solc_ast_generic_type_create(plain_type_pos, cur.value,
                                          generic_type_list);

    goto process_ptrs;
  }

  type = solc_ast_type_plain_create(plain_type_pos, cur.value);

process_ptrs:
  for (; ptr_num > 0; ptr_num--)
    type = solc_ast_type_pointer_create(ptr_start + ptr_num - 1, type);

  return type;
}

solc_ast_t *solc_parser_parse_type_funcptr(solc_parser_t *parser)
{
  sz funcptr_pos = parser->pos;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  solc_ast_t *func_arglist = solc_parser_parse_func_arglist(parser);

  if (solc_parser_peek(parser, parser->pos) != SOLC_TOKENTYPE_MINUS)
    return solc_ast_type_funcptr_create(funcptr_pos, nullptr, func_arglist);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_MINUS);
  VERIFY_WHITESPACE(parser, parser->pos,
                    parser->tokens[parser->pos].has_whitespace_after, false,
                    SOLC_TOKENTYPE_RARROW);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RARROW);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *type_raw = solc_parser_parse_type_raw(parser);
  return solc_ast_type_funcptr_create(funcptr_pos, type_raw, func_arglist);
}
