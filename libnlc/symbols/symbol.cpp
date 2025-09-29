#include "symbols/symbol.hpp"
#include <algorithm>

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
Struct::add_field (const std::string &name, PositionedType field)
{
  fields[name] = field;
}

const std::shared_ptr<Type> &
Struct::get_field (const std::string &name) const
{
  return fields.at (name).handle;
}

std::vector<std::shared_ptr<Type>>
Struct::get_fields_sorted ()
{
  std::vector<PositionedType> posvars{};
  for (auto &kv : fields)
    posvars.push_back (kv.second);

  if (posvars.empty ())
    return {};

  std::sort (posvars.begin (), posvars.end (),
             [] (const PositionedType &a, const PositionedType &b) {
               return a.pos < b.pos;
             });

  std::vector<std::shared_ptr<Type>> out{};
  for (auto &pv : posvars)
    out.emplace_back (pv.handle);

  return out;
}

}
