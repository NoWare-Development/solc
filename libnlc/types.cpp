#include "types.hpp"
#include <memory>

namespace nlc
{

std::shared_ptr<Type>
Type::copy () const
{
  auto new_type = std::make_shared<Type> ();
  new_type->type_path = type_path;
  new_type->array_sizes = array_sizes;
  new_type->arguments = arguments;
  new_type->type_name = type_name;
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

const std::vector<std::shared_ptr<AST>> &
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

const std::vector<std::string> &
Type::get_type_path () const
{
  return type_path;
}

const std::string &
Type::get_type_name () const
{
  return type_name;
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

      if (to->get_type_name () != from->get_type_name ())
        return nullptr;

      const auto &from_path = from->get_type_path ();
      const auto &to_path = to->get_type_path ();

      auto from_path_size = from_path.size ();
      auto to_path_size = to_path.size ();
      if (from_path_size != to_path_size)
        return nullptr;

      for (size_t i = 0; i < from_path_size; i++)
        {
          if (from_path.at (i) != to_path.at (i))
            return nullptr;
        }

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
