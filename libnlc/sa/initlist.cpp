#include "libnlc/util.hpp"
#include "sa/sa.hpp"
#include "symbols/symbol.hpp"
#include "types.hpp"
#include <unordered_set>

namespace nlc
{

bool
SemanticAnalyzer::verify_type_of_initlist (const AST &initlist,
                                           const std::shared_ptr<Type> &type,
                                           size_t type_tok_pos)
{
  if (initlist.children.empty ())
    return true;

  if (type->is_explicit_pointer () || type->is_integer ()
      || type->is_floating ())
    {
      if (initlist.children.size () > 1)
        {
          add_error (SAErrorType::INITLIST_WRONG_NUMBER_OF_ENTRIES,
                     initlist.token_position);
          return false;
        }

      const auto &single_init = initlist.children.at (0);
      if (single_init.type == ASTType::INITLIST_ENTRY_EXPLICIT)
        {
          add_error (SAErrorType::INITLIST_EXPLICIT_IN_ANON,
                     single_init.token_position);
          return false;
        }

      const auto &entry = single_init.children.at (0);
      if (entry.type == ASTType::INITLIST)
        {
          add_error (SAErrorType::INITLIST_MULTIPLE_INITS_IN_ANON,
                     entry.token_position);
          return false;
        }

      auto expr_type = get_type_from_expr_ast (entry);
      if (!can_convert_types (expr_type, type))
        {
          add_error (SAErrorType::CANNOT_CONVERT_TYPES_EXPR,
                     entry.token_position);
          return false;
        }

      return true;
    }

  auto symbol = const_cast<SymbolTable::Symbol *> (
      _symbol_table->get_entry (type->get_type_path (), type->type_name));
  auto kind = symbol->get_kind ();
  if (kind != SymbolTable::Symbol::Kind::STRUCT
      && kind != SymbolTable::Symbol::Kind::UNION)
    {
      add_error (SAErrorType::SYMBOL_REFERED_BY_TYPE_IS_NOT_STRUCT_OR_UNION,
                 type_tok_pos, (std::vector<std::shared_ptr<Type>>){ type });
      return false;
    }

  auto symboldata = symbol->get_symbol ();
  if (symboldata == nullptr)
    {
      add_error (SAErrorType::SYMBOL_REFERED_BY_TYPE_DOES_NOT_CONTAIN_DATA,
                 type_tok_pos, (std::vector<std::shared_ptr<Type>>){ type });
      return false;
    }

  Struct *structdata = static_cast<Struct *> (symboldata.get ());
  auto fields_sorted = structdata->get_fields_sorted ();

  auto initlist_entries_count = initlist.children.size ();
  if (initlist_entries_count >= fields_sorted.size ())
    {
      add_error (SAErrorType::INITLIST_WRONG_NUMBER_OF_ENTRIES,
                 initlist.token_position);
      return false;
    }

  enum class __selected_entry_type
  {
    NONE,
    ANON,
    EXPLICIT,
  } selected
      = __selected_entry_type::NONE;
  std::unordered_set<std::string> initialized_field_names{};
  for (size_t i = 0; i < initlist_entries_count; i++)
    {
      const auto &entry = initlist.children.at (i);

      if (entry.type == ASTType::INITLIST_ENTRY_EXPLICIT
          && initialized_field_names.find (entry.value)
                 != initialized_field_names.end ())
        {
          add_error (SAErrorType::INITLIST_EXPLICIT_REINIT,
                     entry.token_position);
          return false;
        }

      std::shared_ptr<Type> init_type{};
      const AST *init_expr;

      switch (selected)
        {
        case __selected_entry_type::NONE:
          {
            switch (entry.type)
              {
              case ASTType::INITLIST_ENTRY:
                {
                  init_type = fields_sorted.at (i);
                  selected = __selected_entry_type::ANON;
                }
                break;
              case ASTType::INITLIST_ENTRY_EXPLICIT:
                {
                  if (!structdata->has_field (entry.value))
                    {
                      add_error (kind == SymbolTable::Symbol::Kind::STRUCT
                                     ? SAErrorType::STRUCT_HAS_NO_FIELD
                                     : SAErrorType::UNION_HAS_NO_FIELD,
                                 entry.token_position);
                      return false;
                    }
                  init_type = structdata->get_field (entry.value);
                  initialized_field_names.insert (entry.value);
                  selected = __selected_entry_type::EXPLICIT;
                }
                break;
              default:
                break;
              }
            init_expr = &entry.children.at (0);
          }
          break;
        case __selected_entry_type::ANON:
          {
            if (entry.type == ASTType::INITLIST_ENTRY_EXPLICIT)
              {
                add_error (SAErrorType::INITLIST_EXPLICIT_AFTER_ANON,
                           entry.token_position);
                return false;
              }
            init_type = fields_sorted.at (i);
            init_expr = &entry.children.at (0);
          }
          break;
        case __selected_entry_type::EXPLICIT:
          {
            if (entry.type == ASTType::INITLIST_ENTRY)
              {
                add_error (SAErrorType::INITLIST_ANON_AFTER_EXPLICIT,
                           entry.token_position);
                return false;
              }
            if (!structdata->has_field (entry.value))
              {
                add_error (kind == SymbolTable::Symbol::Kind::STRUCT
                               ? SAErrorType::STRUCT_HAS_NO_FIELD
                               : SAErrorType::UNION_HAS_NO_FIELD,
                           entry.token_position);
                return false;
              }
            init_type = fields_sorted.at (i);
            init_expr = &entry.children.at (0);
            initialized_field_names.insert (entry.value);
          }
          break;
        }

      if (init_expr == nullptr)
        return false;

      if (init_expr->type == ASTType::INITLIST
          && !verify_type_of_initlist (*init_expr, init_type,
                                       entry.token_position))
        return false;

      auto init_expr_type = get_type_from_expr_ast (*init_expr);
      if (!can_convert_types (init_expr_type, init_type))
        {
          add_error (SAErrorType::CANNOT_CONVERT_TYPES_EXPR,
                     entry.token_position,
                     (std::vector<std::shared_ptr<Type>>){ init_expr_type,
                                                           init_type });
          return false;
        }
    }

  return true;
}

}
