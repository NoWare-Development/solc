#include "libsolc/util.hpp"
#include "parser/ast.hpp"
#include "sa/sa.hpp"
#include "symbols/symbol.hpp"

namespace solc
{

void SemanticAnalyzer::populate_symbols()
{
  populate_enums(enum_asts);
  populate_structs(struct_asts);
  populate_unions(union_asts);
  populate_funcs(func_asts);
}

void SemanticAnalyzer::populate_enums(
  const std::vector<std::pair<Enum *, const AST *> > enum_asts)
{
  for (auto &e : enum_asts) {
    auto enum_symbol = e.first;
    auto enum_ast = e.second;

    struct PendingElement {
      const std::string name;
      Enum::Element element;
      const AST *ast_ptr;

      PendingElement(const std::string &name, Enum::Element element,
                     const AST *ast_ptr)
        : name(name)
        , element(std::move(element))
        , ast_ptr(ast_ptr)
      {
      }
    };

    std::vector<PendingElement> elements{};
    _current_symbol_scope = &_symbol_scope_bindings.at((void *)enum_ast);

    if (enum_ast->children.empty()) {
      add_error(SAErrorType::ENUM_IS_EMPTY, enum_ast->token_position);
      continue;
    }

    for (const auto &child : enum_ast->children) {
      if (child.type != ASTType::ENUM_ELEMENT)
        NOREACH();

      const auto &elem_name = child.value;

      if (!child.children.empty()) {
        const auto &expr = child.children.at(0);
        auto expr_comptime = is_expr_known_at_comptime(expr);
        if (expr_comptime.type != SAErrorType::NONE) {
          add_error(expr_comptime);
          continue;
        }

        auto expr_type = get_type_from_expr_ast(expr);
        if (expr_type == nullptr)
          continue;

        if (!expr_type->is_integer()) {
          add_error(SAErrorType::ENUM_ELEM_EXPR_NOT_INT, child.token_position);
          continue;
        }

        size_t expr_result = get_expr_comptime_ivalue(expr);

        elements.emplace_back(elem_name, Enum::Element::create(expr_result),
                              &child);
      } else {
        if (elements.empty()) {
          elements.emplace_back(elem_name, Enum::Element::create(), &child);
        } else {
          const auto last = std::prev(elements.end());
          elements.emplace_back(elem_name, Enum::Element::create(last->element),
                                &child);
        }
      }
    }

    for (auto e : elements) {
      if (enum_symbol->has_element(e.name)) {
        add_error(SAErrorType::ENUM_ELEM_REDEF, e.ast_ptr->token_position);
        continue;
      }

      enum_symbol->add_element(e.name, std::move(e.element));
    }
  }
}

void SemanticAnalyzer::populate_structs(
  const std::vector<std::pair<Struct *, const AST *> > asts)
{
}

void SemanticAnalyzer::populate_unions(
  const std::vector<std::pair<Union *, const AST *> > asts)
{
}

void SemanticAnalyzer::populate_funcs(
  const std::vector<std::pair<Function *, const AST *> > asts)
{
}

}
