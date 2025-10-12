#include "sa/scope.hpp"
#include <cstdlib>
#include <vector>

namespace solc
{

bool
Scope::has_alias (const std::string &name) const
{
  return aliases.find (name) != aliases.end ();
}

void
Scope::add_alias (const std::string &name, std::shared_ptr<Type> t)
{
  aliases[name] = t;
}

const std::shared_ptr<Type> &
Scope::get_alias (const std::string &name) const
{
  return aliases.at (name);
}

bool
Scope::has_variable (const std::string &name) const
{
  return variables.find (name) != variables.end ();
}

void
Scope::add_variable (const std::string &name, std::shared_ptr<Variable> v)
{
  variables[name] = v;
}

const std::shared_ptr<Variable> &
Scope::get_variable (const std::string &name) const
{
  return variables.at (name);
}

bool
Scope::has_function (const std::string &name) const
{
  return functions.find (name) != functions.end ();
}

void
Scope::add_function (const std::string &name, std::shared_ptr<Function> f)
{
  functions[name] = f;
}

const std::shared_ptr<Function> &
Scope::get_function (const std::string &name) const
{
  return functions.at (name);
}

bool
Scope::has_struct (const std::string &name) const
{
  return structs.find (name) != structs.end ();
}

void
Scope::add_struct (const std::string &name, std::shared_ptr<Struct> s)
{
  structs[name] = s;
}

const std::shared_ptr<Struct> &
Scope::get_struct (const std::string &name) const
{
  return structs.at (name);
}

bool
Scope::has_enum (const std::string &name) const
{
  return enums.find (name) != enums.end ();
}

void
Scope::add_enum (const std::string &name, std::shared_ptr<Enum> e)
{
  enums[name] = e;
}

const std::shared_ptr<Enum> &
Scope::get_enum (const std::string &name) const
{
  return enums.at (name);
}

bool
Scope::has_union (const std::string &name) const
{
  return unions.find (name) != unions.end ();
}

void
Scope::add_union (const std::string &name, std::shared_ptr<Union> u)
{
  unions[name] = u;
}

const std::shared_ptr<Union> &
Scope::get_union (const std::string &name) const
{
  return unions.at (name);
}

ScopeStack::ScopeStack () { _scopes = new Scope[_capacity]; }

ScopeStack::ScopeStack (size_t reserved) : _capacity (reserved)
{
  _scopes = new Scope[_capacity];
}

ScopeStack::~ScopeStack () { delete[] _scopes; }

void
ScopeStack::push (Scope &&scope)
{
  if (_length >= _capacity)
    resize ();

  _scopes[_length++] = scope;
}

Scope
ScopeStack::pop ()
{
  if (_length <= 0)
    return {};

  _length--;
  return _scopes[_length];
}

size_t
ScopeStack::length () const
{
  return _length;
}

Scope *
ScopeStack::top ()
{
  if (_length == 0)
    return nullptr;
  return &_scopes[_length - 1];
}

Scope *
ScopeStack::bottom ()
{
  if (_length == 0)
    return nullptr;
  return &_scopes[0];
}

std::vector<std::string>
ScopeStack::construct_path (const Scope *terminator) const
{
  std::vector<std::string> out{};

  for (size_t i = 0; i < _length; i++)
    {
      const auto scope = &_scopes[i];

      if (scope->kind == Scope::Kind::STRUCT
          || scope->kind == Scope::Kind::UNION)
        {
          out.push_back (scope->name);
        }

      if (terminator != nullptr && scope == terminator)
        break;
    }

  return out;
}

void
ScopeStack::resize ()
{
  auto old_scopes = _scopes;
  _scopes = new Scope[_capacity * 2];

  for (size_t i = 0; i < _capacity; i++)
    {
      _scopes[i] = std::move (old_scopes[i]);
    }

  delete[] old_scopes;
  _capacity *= 2;
}

}
