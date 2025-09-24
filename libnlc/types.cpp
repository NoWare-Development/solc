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
  return group == BuiltinTypeGroup::INT || group == BuiltinTypeGroup::UINT;
}

bool
Type::is_floating () const
{
  return get_builtin_type_group (builtin_type) == BuiltinTypeGroup::FLOAT;
}

bool
Type::is_complex () const
{
  return get_builtin_type_group (builtin_type) == BuiltinTypeGroup::COMPLEX;
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
  if (from->is_array () && !to->is_explicit_pointer () && !to->is_integer ())
    {
      return nullptr;
    }

  if (from->is_explicit_pointer () && !to->is_integer ()
      && !to->is_explicit_pointer ())
    return nullptr;

  if (get_converted_builtin_type (from->builtin_type, to->builtin_type)
      == BuiltinType::UNK)
    {
      if (from->is_complex () && to->is_complex ())
        {
          if (from->get_type_name () != to->get_type_name ())
            return nullptr;

          const auto &from_path = from->get_type_path ();
          const auto &to_path = to->get_type_path ();
          if (from_path.size () != to_path.size ())
            return nullptr;

          auto path_size = from_path.size ();
          for (size_t i = 0; i < path_size; i++)
            {
              if (from_path.at (i) != to_path.at (i))
                return nullptr;
            }
        }
      else
        return nullptr;
    }

  return to;
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
