#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/parser_context.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_struct(solc_parser_t *parser)
{
  sz struct_pos = parser->pos++;

  VERIFY_POS(parser, parser->pos);
  solc_token_t cur = parser->tokens[parser->pos];
  VERIFY_TOKEN(parser, parser->pos, cur.type, SOLC_TOKENTYPE_ID);
  const char *struct_name = cur.value;

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  cur = parser->tokens[parser->pos];

  solc_ast_t *generic_placeholder_type_list_ast = nullptr;
  if (cur.type == SOLC_TOKENTYPE_LARROW) {
    generic_placeholder_type_list_ast =
      solc_parser_parse_generic_placeholder_type_list(parser);
    VERIFY_POS(parser, parser->pos);
    cur = parser->tokens[parser->pos];
  }

  VERIFY_TOKEN(parser, parser->pos, cur.type, SOLC_TOKENTYPE_LCBRACK);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *struct_ast =
    generic_placeholder_type_list_ast != nullptr ?
      solc_ast_generic_struct_create(struct_pos, struct_name,
                                     generic_placeholder_type_list_ast) :
      solc_ast_struct_create(struct_pos, struct_name);

  while (parser->pos < parser->tokens_num) {
    // NOTE: Cannot check if 'child_ast' is a null-pointer or not,
    // because if parse function failed, it can output a null-pointer.
    b8 parsed_child = false;
    cur = parser->tokens[parser->pos];
    solc_ast_t *child_ast = nullptr;
    if (cur.type == SOLC_TOKENTYPE_RCBRACK)
      break;
    else if (cur.type == SOLC_TOKENTYPE_ID) {
      parser_struct_func_t struct_func =
        parser_context_get_struct_func(cur.value);
      if (struct_func != nullptr) {
        child_ast = struct_func(parser);
        parsed_child = true;
      }
    }

    if (!parsed_child)
      child_ast = solc_parser_parse_decldef(parser);

    if (struct_ast->type == SOLC_AST_TYPE_NONE_STRUCT) {
      solc_ast_struct_add_child(struct_ast, child_ast);
    } else {
      solc_ast_generic_struct_append_child(struct_ast, child_ast);
    }
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);
  parser->pos++;

  return struct_ast;
}
