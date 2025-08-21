#pragma once

#include <cstddef>
#include <cstring>
#include <semantic-analyzer/types.hpp>
#include <string>
#include <unordered_map>

namespace nlc::sa
{

enum ScopeKind
{
  SCOPE_KIND_GLOBAL,
  SCOPE_KIND_FUNC,
  SCOPE_KIND_METHOD,
  SCOPE_KIND_STRUCT,
  SCOPE_KIND_UNION,
};

struct Scope
{
  std::unordered_map<std::string, Type> aliased_type_table{};
  std::unordered_map<std::string, Struct> struct_table{};
  std::unordered_map<std::string, Union> union_table{};
  std::unordered_map<std::string, Enum> enum_table{};

  std::unordered_map<std::string, Variable> variable_table{};
  std::unordered_map<std::string, Func> func_table{};

  ScopeKind kind{};
};

struct ScopeStack
{
public:
  using iterator = Scope *;
  using const_iterator = const Scope *;

  ScopeStack ()
  {
    _scopes = new Scope[8];
    _capacity = 8;
  }
  ~ScopeStack ()
  {
    if (_scopes != nullptr)
      {
        delete[] _scopes;
      }
  }

  void
  push (Scope &scope)
  {
    if (_stack_size >= _capacity)
      {
        resize ();
      }

    _scopes[_stack_size] = std::move (scope);
    _stack_size += 1;
  }

  Scope
  pop ()
  {
    if (_stack_size <= 0)
      return {};

    _stack_size -= 1;
    return _scopes[_stack_size];
  }

  size_t
  size () const
  {
    return _stack_size;
  }

  iterator
  begin ()
  {
    return &_scopes[0];
  }

  iterator
  end ()
  {
    return &_scopes[_stack_size];
  }

  const_iterator
  cbegin ()
  {
    return &_scopes[0];
  }

  const_iterator
  cend ()
  {
    return &_scopes[_stack_size];
  }

  Scope &
  top ()
  {
    return _scopes[_stack_size - 1];
  }

  Scope &
  bottom ()
  {
    return _scopes[0];
  }

private:
  Scope *_scopes = nullptr;

  size_t _capacity = 0;
  size_t _stack_size = 0;

  void
  resize ()
  {
    auto newscopes = new Scope[_capacity * 2];
    std::memcpy (static_cast<void *> (newscopes),
                 static_cast<void *> (_scopes), sizeof (Scope) * _stack_size);

    delete[] _scopes;
    _scopes = newscopes;

    _capacity *= 2;
  }
};

}
