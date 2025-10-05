#include "config/config.hpp"
#include "libnlc/util.hpp"
#include "sa/sa.hpp"
#include "types.hpp"
#include <vector>

namespace nlc
{

std::shared_ptr<Type>
SemanticAnalyzer::get_type_from_type_ast (const AST &type)
{
  size_t pointer_indirection = 0;
  std::vector<std::shared_ptr<AST>> array_sizes{};

  auto *current = const_cast<AST *> (&type);
  while (true)
    {
      switch (current->type)
        {
        case ASTType::TYPE_POINTER:
          {
            pointer_indirection += 1;
            current = &current->children.at (0);
          }
          break;

        case ASTType::TYPE_ARRAY:
          {
            if (current->children.size () > 1)
              {
                auto comptime_error
                    = is_expr_known_at_comptime (current->children.at (0));
                if (comptime_error.type != SAErrorType::NONE)
                  {
                    add_error (comptime_error);
                    add_error (SAErrorType::ARRAY_SIZE_NOT_COMPTIME,
                               current->children.at (0).token_position);
                    return nullptr;
                  }

                auto type_of_expr
                    = get_type_from_expr_ast (current->children.at (0));
                if (type_of_expr == nullptr)
                  return nullptr;
                else if (!type_of_expr->is_integer ())
                  {
                    add_error (SAErrorType::ARRAY_SIZE_NOT_INTEGER,
                               current->children.at (0).token_position);
                    return nullptr;
                  }

                array_sizes.push_back (
                    std::make_shared<AST> (current->children.at (0)));
                current = &current->children.at (1);
              }
            else
              {
                array_sizes.push_back (nullptr);
                current = &current->children.at (0);
              }
          }
          break;

        case ASTType::NAMESPACE:
          TODO ("Type from namespace");

        case ASTType::TYPE_FUNCPTR:
          TODO ("Function pointer");

        case ASTType::TYPE_PLAIN:
          {
            std::shared_ptr<Type> type;

            const auto &value = current->value;
            if (is_basic_type (value))
              {
                // Basic types
                type = get_basic_type (value);
                type->pointer_indirection += pointer_indirection;
                type->array_sizes.insert (type->array_sizes.end (),
                                          array_sizes.begin (),
                                          array_sizes.end ());
                return type;
              }

            // TODO: Check for imported symbols.
            for (const auto &scope : *_scope_stack)
              {
                // Check for aliases
                if (scope.has_alias (value))
                  {
                    type = scope.get_alias (value)->copy ();
                    type->pointer_indirection += pointer_indirection;
                    type->array_sizes.insert (type->array_sizes.end (),
                                              array_sizes.begin (),
                                              array_sizes.end ());

                    return type;
                  }

                // Check for structs, enums, unions,
                BuiltinType builtin_type = BuiltinType::UNK;
                if (scope.has_struct (value))
                  builtin_type = BuiltinType::STRUCT;
                else if (scope.has_enum (value))
                  builtin_type = BuiltinType::ENUM;
                else if (scope.has_union (value))
                  builtin_type = BuiltinType::UNION;
                else
                  continue;

                auto path = _scope_stack->construct_path (&scope);
                type = Type::create_complex (builtin_type, value, path);

                type->pointer_indirection += pointer_indirection;
                type->array_sizes.insert (type->array_sizes.end (),
                                          array_sizes.begin (),
                                          array_sizes.end ());

                return type;
              }

            add_error (SAErrorType::UNDEFINED_TYPE, current->token_position);
            return nullptr;
          }
          break;

        default:
          NOREACH ();
        }
    }

  return nullptr;
}

bool
SemanticAnalyzer::is_basic_type (const std::string &type) const
{
  return _basic_types.find (type) != _basic_types.end ()
         || _architecture_dependent_types.find (type)
                != _architecture_dependent_types.end ();
}

std::shared_ptr<Type>
SemanticAnalyzer::get_basic_type (const std::string &type) const
{
  if (!is_basic_type (type))
    return nullptr;

  if (_basic_types.find (type) != _basic_types.end ())
    return _basic_types.at (type)->copy ();

  return _architecture_dependent_types.at (type)->copy ();
}

bool
SemanticAnalyzer::does_type_exist (const std::string &type) const
{
  if (is_basic_type (type))
    return true;

  for (const auto &scope : *_scope_stack)
    {
      if (scope.has_alias (type) || scope.has_struct (type)
          || scope.has_enum (type) || scope.has_union (type))
        return true;
    }

  // TODO: Check for imported symbols.

  return false;
}

void
SemanticAnalyzer::populate_architecture_dependent_types ()
{
  switch (Config::the ().get_output_arch ())
    {
    case Config::OutputArch::ARCH_X86:
      _architecture_dependent_types["size_t"]
          = Type::create_basic (BuiltinType::ULONG);
      _architecture_dependent_types["uptr"]
          = Type::create_basic (BuiltinType::ULONG);

      _architecture_dependent_types["ssize_t"]
          = Type::create_basic (BuiltinType::LONG);
      _architecture_dependent_types["off_t"]
          = Type::create_basic (BuiltinType::LONG);
      _architecture_dependent_types["iptr"]
          = Type::create_basic (BuiltinType::LONG);
      break;

    case Config::OutputArch::ARCH_AMD64:
    case Config::OutputArch::ARCH_ARM64:
      _architecture_dependent_types["size_t"]
          = Type::create_basic (BuiltinType::UINT);
      _architecture_dependent_types["uptr"]
          = Type::create_basic (BuiltinType::UINT);

      _architecture_dependent_types["ssize_t"]
          = Type::create_basic (BuiltinType::INT);
      _architecture_dependent_types["off_t"]
          = Type::create_basic (BuiltinType::INT);
      _architecture_dependent_types["iptr"]
          = Type::create_basic (BuiltinType::INT);
      break;
    }
}

bool
SemanticAnalyzer::is_valid_typespec (const std::string &ts) const
{
  auto lowercase = util::string_lowercase (ts);
  return _typespec_to_type.find (lowercase) != _typespec_to_type.end ();
}

std::shared_ptr<Type>
SemanticAnalyzer::get_typespec (const std::string &ts) const
{
  if (!is_valid_typespec (ts))
    return nullptr;

  auto lowercase = util::string_lowercase (ts);
  return _typespec_to_type.at (lowercase)->copy ();
}

bool
SemanticAnalyzer::is_valid_literal (const std::string &literal) const
{
  return _literal_type.find (literal) != _literal_type.end ();
}

std::shared_ptr<Type>
SemanticAnalyzer::get_literal_type (const std::string &literal) const
{
  if (!is_valid_literal (literal))
    return nullptr;
  return _literal_type.at (literal);
}

}
