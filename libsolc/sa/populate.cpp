#include "sa/sa.hpp"

namespace solc
{

void
SemanticAnalyzer::populate_symbols ()
{
  for (auto ast_ptr : struct_union_asts)
    {
      auto &symbol_scope = _symbol_scope_bindings[(void *)ast_ptr];
    }
}

}
