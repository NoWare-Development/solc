#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_loop(solc_parser_t *parser)
{
  sz loop_pos = parser->pos++;
  solc_ast_t *attrib_list = solc_parser_parse_attribute_list_optional(parser);
  VERIFY_POS(parser, parser->pos);
  solc_ast_t *stmt = solc_parser_parse_stmt(parser);
  return solc_ast_stmt_loop_create(loop_pos, stmt, attrib_list);
}
