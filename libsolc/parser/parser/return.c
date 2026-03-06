#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_return(solc_parser_t *parser)
{
  sz return_pos = parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *expr = nullptr;
  if (parser->tokens[parser->pos].type != SOLC_TOKENTYPE_SEMI) {
    expr = solc_parser_parse_expr(parser, false);
    VERIFY_POS(parser, parser->pos);
  }

  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_SEMI);
  parser->pos++;

  return solc_ast_stmt_return_create(return_pos, expr);
}
