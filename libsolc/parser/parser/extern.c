#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/defs.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_extern(solc_parser_t *parser)
{
  parser->pos++;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const char *extern_name = parser->tokens[parser->pos].value;
  sz extern_pos = parser->pos;

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_COLON);

  solc_ast_t *extern_ast;

  if (parser->tokens[parser->pos].has_whitespace_after) {
    parser->pos++;

    solc_ast_t *type_ast = solc_parser_parse_type(parser);
    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_SEMI);

    parser->pos++;

    extern_ast =
      solc_ast_extern_vardecl_create(extern_pos, extern_name, type_ast);
  } else {
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
                 SOLC_TOKENTYPE_SEMI);

    parser->pos++;

    extern_ast = solc_ast_extern_func_create(extern_pos, extern_name, type_ast,
                                             arglist_ast);
  }

  return extern_ast;
}
