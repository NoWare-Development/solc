#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_export()
{
  _pos++;
  VERIFY_POS(_pos);

  auto exported_func = parse_function();
  exported_func.type = ASTType::EXPORTED_FUNC;

  return exported_func;
}

}
