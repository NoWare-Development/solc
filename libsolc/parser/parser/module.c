#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_module(solc_parser_t *parser)
{
  solc_token_t cur = parser->tokens[parser->pos];
  VERIFY_TOKEN(parser, parser->pos, cur.type, SOLC_TOKENTYPE_ID);

  sz module_pos = parser->pos++;

  solc_ast_t *submodule_ast = nullptr;
  const solc_token_t *next_token = solc_parser_peek_token(parser, parser->pos);
  if (next_token != nullptr && next_token->type == SOLC_TOKENTYPE_COLON &&
      !next_token->has_whitespace_after &&
      solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_COLON) {
    parser->pos += 2;
    submodule_ast = solc_parser_parse_module(parser);
  }

  return solc_ast_module_create(module_pos, cur.value, submodule_ast);
}
