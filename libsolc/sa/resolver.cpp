#include "libsolc/logger.hpp"
#include "libsolc/util.hpp"
#include "sa/sa.hpp"
#include "symbols/symbol.hpp"
#include "types.hpp"

namespace solc
{

std::shared_ptr<Type>
SemanticAnalyzer::resolve_type(const std::string &name) const
{
  if (is_basic_type(name)) {
    return get_basic_type(name)->copy();
  }

  auto scope = _current_symbol_scope;
  while (scope != nullptr) {
    if (scope->has_symbol(name)) {
      auto symbol = scope->get_symbol(name).get();
      auto symbol_type = symbol->get_symbol_type();
      BuiltinType builtin_type = BuiltinType::UNK;
      switch (symbol_type) {
      case Symbol::Type::ALIAS: {
        auto alias_symbol = (Alias *)symbol;
        return alias_symbol->get_type();
      }
      case Symbol::Type::ENUM:
        builtin_type = BuiltinType::ENUM;
        break;
      case Symbol::Type::STRUCT:
        builtin_type = BuiltinType::STRUCT;
        break;
      case Symbol::Type::UNION:
        builtin_type = BuiltinType::UNION;
        break;
      default:
        break;
      }

      if (builtin_type != BuiltinType::UNK) {
        return Type::create_complex(builtin_type, symbol);
      }
    }

    scope = scope->get_parent();
  }
  return nullptr;
}

std::shared_ptr<Type>
SemanticAnalyzer::resolve_type(const std::vector<std::string> &ns,
                               const std::string &name) const
{
  // TODO: Support for module resolution (file imports)

  if (ns.empty())
    NOREACH();

  // Find all valid scopes that contain namespace entry
  std::vector<SymbolScope *> valid_entry_scopes{};
  const auto &entry = ns.at(0);
  for (auto entry_scope = _current_symbol_scope; entry_scope != nullptr;
       entry_scope = entry_scope->get_parent()) {
    if (entry_scope->has_symbol(entry))
      valid_entry_scopes.push_back(entry_scope);
  }

  // Go through all valid entry scopes and try to resolve namespace
  for (auto scope : valid_entry_scopes) {
    // Set current symbol to namespace entry
    auto current_symbol = scope->get_symbol(entry).get();

    // Try to resolve namespace
    const auto ns_len = ns.size();
    for (size_t i = 1; i < ns_len && current_symbol != nullptr; i++) {
      const auto &ns_part = ns.at(i);

      auto current_symbol_type = current_symbol->get_symbol_type();
      switch (current_symbol_type) {
      case Symbol::Type::STRUCT:
      case Symbol::Type::UNION: {
        auto struct_symbol = (Struct *)current_symbol;
        // NOTE: Searching for enums is not allowed because enums
        // cannot contain structs, unions, other enums or aliases.
        if (struct_symbol->has_struct(ns_part))
          current_symbol = struct_symbol->get_struct(ns_part).get();
        else if (struct_symbol->has_union(ns_part))
          current_symbol = struct_symbol->get_union(ns_part).get();
        else if (struct_symbol->has_alias(ns_part)) {
          auto alias = struct_symbol->get_alias(ns_part);
          auto aliased_type = alias->get_type();
          if (aliased_type == nullptr) {
            info("Type of alias \"" << ns_part << "\" is not defined");
            current_symbol = nullptr;
            break;
          }

          auto bound_symbol = aliased_type->get_symbol_binding();
          if (bound_symbol == nullptr) {
            info("Type of alias \"" << ns_part << "\" has no symbol binding");
            current_symbol = nullptr;
            break;
          }

          auto bound_symbol_type = bound_symbol->get_symbol_type();
          if (bound_symbol_type != Symbol::Type::STRUCT &&
              bound_symbol_type != Symbol::Type::UNION) {
            current_symbol = nullptr;
            break;
          }

          current_symbol = bound_symbol;
        } else
          current_symbol = nullptr;
      } break;
      case Symbol::Type::ALIAS: {
        auto aliased_type = ((Alias *)current_symbol)->get_type();
        if (aliased_type == nullptr) {
          current_symbol = nullptr;
          break;
        }

        auto bound_symbol = aliased_type->get_symbol_binding();
        if (bound_symbol == nullptr) {
          current_symbol = nullptr;
          break;
        }

        auto bound_symbol_type = bound_symbol->get_symbol_type();
        if (bound_symbol_type != Symbol::Type::STRUCT &&
            bound_symbol_type != Symbol::Type::UNION) {
          current_symbol = nullptr;
          break;
        }

        current_symbol = bound_symbol;
      } break;

      default:
        current_symbol = nullptr;
        break;
      }
    }

    if (current_symbol == nullptr)
      continue;

    switch (current_symbol->get_symbol_type()) {
    case Symbol::Type::UNION:
    case Symbol::Type::STRUCT: {
      auto struct_symbol = (Struct *)current_symbol;
      Symbol *found_symbol{};
      if (struct_symbol->has_struct(name))
        found_symbol = struct_symbol->get_struct(name).get();
      else if (struct_symbol->has_union(name))
        found_symbol = struct_symbol->get_union(name).get();
      else if (struct_symbol->has_alias(name))
        found_symbol = struct_symbol->get_alias(name).get();
      else
        break;

      auto builtin_type = BuiltinType::UNK;
      switch (found_symbol->get_symbol_type()) {
      case Symbol::Type::STRUCT:
        builtin_type = BuiltinType::STRUCT;
        break;

      case Symbol::Type::UNION:
        builtin_type = BuiltinType::UNION;
        break;

      case Symbol::Type::ENUM:
        builtin_type = BuiltinType::ENUM;
        break;

      case Symbol::Type::ALIAS: {
        return ((Alias *)found_symbol)->get_type();
      } break;

      default:
        break;
      }

      if (builtin_type == BuiltinType::UNK)
        break;

      return Type::create_complex(builtin_type, found_symbol);
    } break;

    case Symbol::Type::ALIAS: {
      auto aliased_type = ((Alias *)current_symbol)->get_type();
      if (aliased_type == nullptr)
        break;

      auto bound_symbol = aliased_type->get_symbol_binding();
      if (bound_symbol == nullptr)
        break;

      auto bound_symbol_type = bound_symbol->get_symbol_type();
      switch (bound_symbol_type) {
      case Symbol::Type::STRUCT:
      case Symbol::Type::UNION: {
        auto struct_symbol = (Struct *)bound_symbol;
        Symbol *found_symbol{};
        if (struct_symbol->has_struct(name))
          found_symbol = struct_symbol->get_struct(name).get();
        else if (struct_symbol->has_union(name))
          found_symbol = struct_symbol->get_union(name).get();
        else if (struct_symbol->has_alias(name))
          found_symbol = struct_symbol->get_alias(name).get();
        else
          break;

        auto builtin_type = BuiltinType::UNK;
        switch (found_symbol->get_symbol_type()) {
        case Symbol::Type::STRUCT:
          builtin_type = BuiltinType::STRUCT;
          break;
        case Symbol::Type::UNION:
          builtin_type = BuiltinType::UNION;
          break;
        case Symbol::Type::ALIAS:
          return ((Alias *)found_symbol)->get_type();
        default:
          break;
        }
        if (builtin_type == BuiltinType::UNK)
          break;

        return Type::create_complex(builtin_type, found_symbol);
      } break;

      default:
        break;
      }
    } break;

    default:
      break;
    }
  }

  return nullptr;
}

}
