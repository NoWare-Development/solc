#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_export()
{
  AST export_(_pos++, ASTType::EXPORT);

  VERIFY_POS(_pos);

  auto top = parse_top(true);

  export_.append(top);

  return export_;
}

}
