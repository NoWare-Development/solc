#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_else(solc_parser_t *parser)
{
  sz else_pos = parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *stmt = solc_parser_parse_stmt(parser);

  return solc_ast_stmt_else_create(else_pos, stmt);
}
