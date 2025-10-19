#include "libsolc/util.hpp"
#include "sa/sa.hpp"

namespace solc
{

void SemanticAnalyzer::generate_symbol_scope_bindings(const AST &root)
{
  for (const auto &child : root.children) {
    switch (child.type) {
    case ASTType::FUNC: {
      const auto &name = child.value;
      if (_current_symbol_scope->has_symbol(name)) {
        add_error(SAErrorType::SYMBOL_REDEF, child.token_position);
        break;
      }

      auto func = Function::create();
      _current_symbol_scope->add_symbol(name, func);
      _symbol_scope_bindings[(void *)&child] = *_current_symbol_scope;
      func_asts.emplace_back(func.get(), &child);
      generate_symbol_scope_bindings(*std::prev(child.children.end()));
    } break;

    case ASTType::ENUM: {
      const auto &name = child.value;
      if (_current_symbol_scope->has_symbol(name)) {
        add_error(SAErrorType::SYMBOL_REDEF, child.token_position);
        break;
      }
      auto e = Enum::create();
      _symbol_scope_bindings[(void *)&child] = _current_symbol_scope;
      _current_symbol_scope->add_symbol(name, e);
      enum_asts.emplace_back(e.get(), &child);
    } break;

    case ASTType::STRUCT: {
      const auto &name = child.value;
      if (_current_symbol_scope->has_symbol(name)) {
        add_error(SAErrorType::SYMBOL_REDEF, child.token_position);
        break;
      }
      auto struct_symbol = Struct::create();

      _current_symbol_scope->add_symbol(name, struct_symbol);
      auto current_bound_symbol = _current_symbol_scope->get_bound_symbol();
      if (current_bound_symbol != nullptr) {
        ((Struct *)current_bound_symbol)->add_struct(name, struct_symbol);
      }

      _symbol_scope_bindings[(void *)&child] =
        SymbolScope(_current_symbol_scope, struct_symbol.get());
      _current_symbol_scope = &_symbol_scope_bindings.at((void *)&child);
      generate_symbol_scope_bindings(child);
      _current_symbol_scope = _current_symbol_scope->get_parent();

      struct_asts.emplace_back(struct_symbol.get(), &child);
    } break;
    case ASTType::UNION: {
      const auto &name = child.value;
      if (_current_symbol_scope->has_symbol(name)) {
        add_error(SAErrorType::SYMBOL_REDEF, child.token_position);
        break;
      }
      auto union_symbol = Union::create();

      _current_symbol_scope->add_symbol(name, union_symbol);
      auto current_bound_symbol = _current_symbol_scope->get_bound_symbol();
      if (current_bound_symbol != nullptr) {
        ((Struct *)current_bound_symbol)->add_union(name, union_symbol);
      }

      _symbol_scope_bindings[(void *)&child] =
        SymbolScope(_current_symbol_scope, union_symbol.get());
      _current_symbol_scope = &_symbol_scope_bindings.at((void *)&child);
      generate_symbol_scope_bindings(child);
      _current_symbol_scope = _current_symbol_scope->get_parent();

      union_asts.emplace_back(union_symbol.get(), &child);
    } break;

    default:
      break;
    }
  }
}

void SemanticAnalyzer::generate_aliases(const AST &root)
{
  for (const auto &child : root.children) {
    switch (child.type) {
    case ASTType::TYPEDEF: {
      const auto &name = child.value;
      for (auto scope = _current_symbol_scope; scope != nullptr;
           scope = scope->get_parent()) {
        if (scope->has_symbol(name)) {
          add_error(SAErrorType::TYPE_REDEF, child.token_position);
          break;
        }
      }

      auto aliased_type = get_type_from_type_ast(child.children.at(0));
      if (aliased_type == nullptr)
        return;

      if (aliased_type->is_array()) {
        add_error(SAErrorType::ARRAY_IN_TYPEDEF,
                  child.children.at(0).token_position);
        break;
      }

      auto alias = Alias::create(aliased_type);

      _current_symbol_scope->add_symbol(name, alias);
      auto current_bound_symbol = _current_symbol_scope->get_bound_symbol();
      if (current_bound_symbol != nullptr) {
        ((Struct *)current_bound_symbol)->add_alias(name, alias);
      }
    } break;

    case ASTType::FUNC: {
      auto last = std::prev(child.children.cend());
      if (last->type != ASTType::STMT_LIST) {
        NOREACH();
      }
      generate_symbol_scope_bindings(*last);
    } break;

    case ASTType::UNION:
    case ASTType::STRUCT: {
      auto symbol_scope = &_symbol_scope_bindings.at((void *)&child);
      if (symbol_scope->get_parent() != _current_symbol_scope) {
        NOREACH();
      }

      _current_symbol_scope = symbol_scope;
      generate_aliases(child);
      _current_symbol_scope = _current_symbol_scope->get_parent();
    } break;

    default:
      break;
    }
  }
}

}
