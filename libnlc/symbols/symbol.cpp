#include "symbols/symbol.hpp"

namespace nlc
{

bool
Enum::has_element (const std::string &name) const
{
  return elements.find (name) != elements.end ();
}

void
Enum::add_element (const std::string &name, Element &&element)
{
  elements[name] = element;
}

const Enum::Element &
Enum::get_element (const std::string &name) const
{
  return elements.at (name);
}

bool
Struct::has_field (const std::string &name) const
{
  return fields.find (name) != fields.end ();
}

void
Struct::add_field (const std::string &name, std::shared_ptr<Variable> field)
{
  fields[name] = field;
}

const std::shared_ptr<Variable> &
Struct::get_field (const std::string &name) const
{
  return fields.at (name);
}

bool
Union::has_field (const std::string &name) const
{
  return fields.find (name) != fields.end ();
}

void
Union::add_field (const std::string &name, std::shared_ptr<Variable> field)
{
  fields[name] = field;
}

const std::shared_ptr<Variable> &
Union::get_field (const std::string &name) const
{
  return fields.at (name);
}

}
