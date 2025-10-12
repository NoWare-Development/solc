#include "libsolc/util.hpp"
#include "parser/ast.hpp"
#include "sa/sa.hpp"
#include <string>
#include <vector>

namespace solc
{

std::shared_ptr<Type>
SemanticAnalyzer::get_type_from_type_ast (const AST &type)
{
  switch (type.type)
    {
    case ASTType::TYPE_PLAIN:
      {
        auto resolved_type = resolve_type (type.value);
        if (resolved_type == nullptr)
          {
            add_error (SAErrorType::UNDEFINED_TYPE, type.token_position);
            return nullptr;
          }
        return resolved_type;
      }
      break;

    case ASTType::NAMESPACE:
      {
        std::vector<std::string> ns{};
        auto ast_ptr = &type;

        while (ast_ptr != nullptr && ast_ptr->type != ASTType::TYPE_PLAIN)
          {
            ns.push_back (ast_ptr->value);
            ast_ptr = &ast_ptr->children.at (0);
          }

        if (ast_ptr == nullptr || ast_ptr->type != ASTType::TYPE_PLAIN)
          {
            NOREACH ();
          }

        const auto &name = ast_ptr->value;
        auto resolved_type = resolve_type (ns, name);
        if (resolved_type == nullptr)
          {
            add_error (SAErrorType::UNDEFINED_TYPE, ast_ptr->token_position);
            return nullptr;
          }
        return resolved_type;
      }
      break;

    case ASTType::TYPE_POINTER:
      {
        auto out = get_type_from_type_ast (type.children.at (0));
        out->pointer_indirection += 1;
        return out;
      }
      break;

    case ASTType::TYPE_ARRAY:
      {
        // TODO: verify that array size is comptime.
        size_t type_pos = 0;
        if (type.children.size () > 1)
          {
            type_pos = 1;
          }

        auto out = get_type_from_type_ast (type.children.at (type_pos));

        if (type.children.size () > 1)
          {
            out->array_sizes.push_back (
                std::make_shared<AST> (type.children.at (0)));
          }
        return out;
      }
      break;

    case ASTType::TYPE_FUNCPTR:
      {
        std::vector<Type::FuncArg> arguments{};

        const auto &arglist = type.children.at (0);
        for (const auto &arg : arglist.children)
          {
            auto argtype = get_type_from_type_ast (arg.children.at (0));
            if (argtype == nullptr)
              return nullptr;

            std::shared_ptr<AST> def_expr{};
            if (arg.type == ASTType::VAR_DEF)
              {
                def_expr = std::make_shared<AST> (arg.children.at (1));
              }
            arguments.emplace_back (argtype, def_expr);
          }

        std::shared_ptr<Type> return_type{};
        if (type.children.size () > 1)
          {
            return_type = get_type_from_type_ast (type.children.at (1));
            if (return_type == nullptr)
              return nullptr;
          }
        else
          {
            return_type = get_basic_type ("void")->copy ();
          }

        return Type::create_function_pointer (return_type, arguments);
      }
      break;

    default:
      NOREACH ();
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
  if (_basic_types.find (type) != _basic_types.end ())
    {
      return _basic_types.at (type);
    }
  else if (_architecture_dependent_types.find (type)
           != _architecture_dependent_types.end ())
    {
      return _architecture_dependent_types.at (type);
    }
  return nullptr;
}

bool
SemanticAnalyzer::is_valid_typespec (const std::string &ts) const
{
  return _typespec_to_type.find (ts) != _typespec_to_type.end ();
}

std::shared_ptr<Type>
SemanticAnalyzer::get_typespec (const std::string &ts) const
{
  return _typespec_to_type.at (ts);
}

bool
SemanticAnalyzer::is_valid_literal (const std::string &literal) const
{
  return _literal_type.find (literal) != _literal_type.end ();
}

std::shared_ptr<Type>
SemanticAnalyzer::get_literal_type (const std::string &literal) const
{
  return _literal_type.at (literal);
}

}
