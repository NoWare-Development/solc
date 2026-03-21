#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/parser_context.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_struct(solc_parser_t *parser)
{
  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const char *struct_name = parser->tokens[parser->pos].value;
  const sz struct_pos = parser->pos;

  parser->pos++;

  VERIFY_POS(parser, parser->pos);
  solc_ast_t *generic_placeholder_type_list =
    parser->tokens[parser->pos].type == SOLC_TOKENTYPE_LARROW ?
      solc_parser_parse_generic_placeholder_type_list(parser) :
      nullptr;

  solc_ast_t *attrib_list = solc_parser_parse_attribute_list_optional(parser);

  solc_ast_t *struct_ast =
    generic_placeholder_type_list != nullptr ?
      solc_ast_generic_struct_create(
        struct_pos, struct_name, generic_placeholder_type_list, attrib_list) :
      solc_ast_struct_create(struct_pos, struct_name, attrib_list);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  parser->pos++;
  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RCBRACK)
      break;

    b8 child_parsed = false;
    solc_ast_t *child_ast = nullptr;

    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_ID) {
      parser_struct_func_t struct_func =
        parser_context_get_struct_func(parser->tokens[parser->pos].value);
      if (struct_func != nullptr) {
        child_ast = struct_func(parser);
        child_parsed = true;
      }
    }

    if (!child_parsed) {
      solc_ast_t *decldef_attrib_list =
        solc_parser_parse_attribute_list_optional(parser);
      child_ast = solc_parser_parse_decldef(parser, decldef_attrib_list);
    }

    if (struct_ast->type == SOLC_AST_TYPE_GENERIC_STRUCT)
      solc_ast_generic_struct_add_child(struct_ast, child_ast);
    else
      solc_ast_struct_add_child(struct_ast, child_ast);
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);

  parser->pos++;

  return struct_ast;
}
