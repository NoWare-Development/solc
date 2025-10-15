#include "types.hpp"
#include "symbols/symbolscope.hpp"
#include <algorithm>
#include <memory>

namespace solc
{

std::shared_ptr<Type>
Type::copy () const
{
  auto new_type = std::make_shared<Type> ();
  new_type->array_sizes = array_sizes;
  new_type->arguments = arguments;
  new_type->symbol_binding = symbol_binding;
  new_type->return_type = return_type;
  new_type->pointer_indirection = pointer_indirection;
  new_type->builtin_type = builtin_type;

  return new_type;
}

bool
Type::is_array () const
{
  return !array_sizes.empty ();
}

bool
Type::is_pointer () const
{
  return is_explicit_pointer () || is_array ();
}

bool
Type::is_explicit_pointer () const
{
  return pointer_indirection > 0;
}

bool
Type::is_integer () const
{
  auto group = get_builtin_type_group (builtin_type);
  return (group == BuiltinTypeGroup::INT || group == BuiltinTypeGroup::UINT)
         && !is_pointer ();
}

bool
Type::is_floating () const
{
  return get_builtin_type_group (builtin_type) == BuiltinTypeGroup::FLOAT
         && !is_pointer ();
}

bool
Type::is_complex () const
{
  return get_builtin_type_group (builtin_type) == BuiltinTypeGroup::COMPLEX
         && !is_pointer ();
}

bool
Type::is_enum () const
{
  return builtin_type == BuiltinType::ENUM;
}

bool
Type::is_function_pointer () const
{
  return pointer_indirection == 1 && builtin_type == BuiltinType::FUNC;
}

const std::vector<size_t> &
Type::get_array_sizes () const
{
  return array_sizes;
}

size_t
Type::get_pointer_indirection () const
{
  return pointer_indirection;
}

size_t
Type::get_full_pointer_indirection () const
{
  return pointer_indirection + array_sizes.size ();
}

Symbol *
Type::get_symbol_binding () const
{
  return symbol_binding;
}

const std::shared_ptr<Type> &
Type::get_return_type () const
{
  return return_type;
}

const std::vector<Type::FuncArg> &
Type::get_function_arguments () const
{
  return arguments;
}

std::string
Type::to_string () const
{
  std::string out{};

  size_t pointer_indirection = get_full_pointer_indirection ()
                               - (builtin_type == BuiltinType::FUNC ? 1 : 0);
  for (size_t i = 0; i < pointer_indirection; i++)
    {
      out += '*';
    }

  switch (builtin_type)
    {
    case BuiltinType::UNK:
      out += "<Unknown type>";
      break;

    case BuiltinType::VOID:
      out += "void";
      break;

    case BuiltinType::UCHAR:
      out += "uchar";
      break;

    case BuiltinType::USHORT:
      out += "ushort";
      break;

    case BuiltinType::UINT:
      out += "uint";
      break;

    case BuiltinType::ULONG:
      out += "ulong";
      break;

    case BuiltinType::CHAR:
      out += "char";
      break;

    case BuiltinType::SHORT:
      out += "short";
      break;

    case BuiltinType::INT:
      out += "int";
      break;

    case BuiltinType::LONG:
      out += "long";
      break;

    case BuiltinType::FLOAT:
      out += "float";
      break;

    case BuiltinType::DOUBLE:
      out += "double";
      break;

    case BuiltinType::BOOL:
      out += "bool";
      break;

    case BuiltinType::STRUCT:
    case BuiltinType::UNION:
    case BuiltinType::ENUM:
      {
        const auto &name = symbol_binding->get_symbol_name ();

        auto current_scope = symbol_binding->get_symbol_scope ();
        std::vector<std::string> ns{};

        while (current_scope != nullptr)
          {
            auto bound_symbol = current_scope->get_bound_symbol ();
            if (bound_symbol != nullptr)
              {
                ns.push_back (bound_symbol->get_symbol_name ());
              }
            current_scope = current_scope->get_parent ();
          }
        std::reverse (ns.begin (), ns.end ());

        for (const auto &s : ns)
          {
            out += s + "::";
          }

        out += name;
      }
      break;

    case BuiltinType::FUNC:
      {
        out += "(";
        size_t arguments_size = arguments.size ();
        for (size_t i = 0; i < arguments_size; i++)
          {
            const auto &arg = arguments.at (i);

            if (i > 0)
              out += ", ";

            out += arg.type->to_string ();
          }

        out += ") -> ";
        out += return_type->to_string ();
      }
      break;
    }

  return out;
}

bool
can_convert_types (const std::shared_ptr<Type> &from,
                   const std::shared_ptr<Type> &to)
{
  return get_converted_type (from, to) != nullptr;
}

std::shared_ptr<Type>
get_converted_type (const std::shared_ptr<Type> &from,
                    const std::shared_ptr<Type> &to)
{
  if (to->is_array ())
    return nullptr;

  if (to->is_explicit_pointer ())
    return from->is_pointer () || from->is_integer () ? to : nullptr;

  if (to->is_complex ())
    {
      if (!from->is_complex ())
        return nullptr;

      if (to->get_symbol_binding () != from->get_symbol_binding ())
        return nullptr;

      return to;
    }

  auto builtin_type
      = get_converted_builtin_type (from->builtin_type, to->builtin_type);
  if (builtin_type == BuiltinType::UNK)
    return nullptr;

  auto new_type = to->copy ();
  new_type->builtin_type = builtin_type;
  return new_type;
}

BuiltinType
get_converted_builtin_type (BuiltinType from, BuiltinType to)
{
  auto from_group = get_builtin_type_group (from);
  auto to_group = get_builtin_type_group (to);

  auto from_size = get_builtin_type_size (from);
  auto to_size = get_builtin_type_size (to);

  switch (to_group)
    {
    case BuiltinTypeGroup::FLOAT:
      {
        switch (from_group)
          {
          case BuiltinTypeGroup::INT:
          case BuiltinTypeGroup::UINT:
            return to;

          case BuiltinTypeGroup::FLOAT:
            return to_size >= from_size ? to : from;

          default:
            break;
          }
      }

    case BuiltinTypeGroup::UINT:
    case BuiltinTypeGroup::INT:
      {
        switch (from_group)
          {
          case BuiltinTypeGroup::FLOAT:
            return from;

          case BuiltinTypeGroup::INT:
          case BuiltinTypeGroup::UINT:
            {
              if (to_size == from_size)
                return to_group == BuiltinTypeGroup::UINT ? to : from;

              return to_size < from_size ? from : to;
            }
            break;

          default:
            break;
          }
      }

    default:
      break;
    }

  return BuiltinType::UNK;
}

}
