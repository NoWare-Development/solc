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
  std::vector<size_t> array_sizes{};

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
          TODO ("Array implementation");

        case ASTType::FROM_MODULE:
          TODO ("Type from module");

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
                return type;
              }
            else
              {
                // TODO: Check for imported symbols.
                for (const auto &scope : *_scope_stack)
                  {
                    // Check for aliases
                    if (scope.has_alias (value))
                      {
                        type = scope.get_alias (value)->copy ();
                        type->pointer_indirection += pointer_indirection;

                        auto got_array_begin = array_sizes.begin ();
                        auto got_array_end = array_sizes.end ();
                        auto type_array_end = type->array_sizes.end ();

                        type->array_sizes.insert (
                            type_array_end, got_array_begin, got_array_end);

                        return type;
                      }

                    // Structs, Enums, Unions,
                    BuiltinType builtin_type = BuiltinType::UNK;
                    if (scope.has_struct (value))
                      {
                        builtin_type = BuiltinType::STRUCT;
                      }
                    else if (scope.has_enum (value))
                      {
                        builtin_type = BuiltinType::STRUCT;
                      }

                    if (builtin_type == BuiltinType::UNK)
                      {
                        return nullptr;
                      }

                    auto path = _scope_stack->construct_path (&scope);
                    type = Type::create_complex (builtin_type, value, path);

                    auto got_array_begin = array_sizes.begin ();
                    auto got_array_end = array_sizes.end ();
                    auto type_array_end = type->array_sizes.end ();

                    type->array_sizes.insert (type_array_end, got_array_begin,
                                              got_array_end);

                    return type;
                  }
              }

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
SemanticAnalyzer::get_basic_type (const std::string &type)
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
  switch (Config::get_instance ()->get_output_arch ())
    {
    case Config::OutputArch::ARCH_X86:
      _architecture_dependent_types["size_t"]
          = Type::create_basic (BuiltinType::ULONG);
      _architecture_dependent_types["uptr"]
          = Type::create_basic (BuiltinType::ULONG);

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

      _architecture_dependent_types["off_t"]
          = Type::create_basic (BuiltinType::INT);
      _architecture_dependent_types["iptr"]
          = Type::create_basic (BuiltinType::INT);
      break;
    }
}

}
