#pragma once

#include <memory>
#include <string>
#include <symbols/symbol.hpp>
#include <types.hpp>
#include <unordered_map>
#include <vector>

namespace solc
{

struct Scope
{
  enum class Kind
  {
    NONE = 0,
    GLOBAL,
    FUNCTION,
    STRUCT,
    UNION,
    ENUM,
  };

  std::unordered_map<std::string, std::shared_ptr<Type>> aliases{};
  std::unordered_map<std::string, std::shared_ptr<Variable>> variables{};
  std::unordered_map<std::string, std::shared_ptr<Function>> functions{};
  std::unordered_map<std::string, std::shared_ptr<Struct>> structs{};
  std::unordered_map<std::string, std::shared_ptr<Enum>> enums{};
  std::unordered_map<std::string, std::shared_ptr<Union>> unions{};

  std::string name{};
  Kind kind{ Kind::NONE };

  Scope () = default;
  Scope (Kind kind, const std::string &name = {}) : name (name), kind (kind) {}

  bool has_alias (const std::string &name) const;
  void add_alias (const std::string &name, std::shared_ptr<Type> t);
  const std::shared_ptr<Type> &get_alias (const std::string &name) const;

  bool has_variable (const std::string &name) const;
  void add_variable (const std::string &name, std::shared_ptr<Variable> v);
  const std::shared_ptr<Variable> &
  get_variable (const std::string &name) const;

  bool has_function (const std::string &name) const;
  void add_function (const std::string &name, std::shared_ptr<Function> f);
  const std::shared_ptr<Function> &
  get_function (const std::string &name) const;

  bool has_struct (const std::string &name) const;
  void add_struct (const std::string &name, std::shared_ptr<Struct> s);
  const std::shared_ptr<Struct> &get_struct (const std::string &name) const;

  bool has_enum (const std::string &name) const;
  void add_enum (const std::string &name, std::shared_ptr<Enum> e);
  const std::shared_ptr<Enum> &get_enum (const std::string &name) const;

  bool has_union (const std::string &name) const;
  void add_union (const std::string &name, std::shared_ptr<Union> u);
  const std::shared_ptr<Union> &get_union (const std::string &name) const;
};

struct ScopeStack
{
public:
  ScopeStack ();
  ScopeStack (size_t reserved);
  ~ScopeStack ();

  void push (Scope &&scope);
  Scope pop ();

  size_t length () const;

  using iterator = Scope *;
  using const_iterator = const Scope *;

  iterator
  begin ()
  {
    return &_scopes[0];
  }

  iterator
  end ()
  {
    return &_scopes[_length];
  }

  const_iterator
  cbegin ()
  {
    return &_scopes[0];
  }

  const_iterator
  cend ()
  {
    return &_scopes[_length];
  }

  Scope *top ();
  Scope *bottom ();

  std::vector<std::string> construct_path (const Scope *terminator
                                           = nullptr) const;

private:
  Scope *_scopes;
  size_t _capacity = 2;
  size_t _length = 0;

  void resize ();
};

}
