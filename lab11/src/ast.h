#pragma once

#include <memory>
#include <vector>
#include <variant>
#include <cstdint>

namespace stewkk::lab11 {

enum class Op {
  kAdd,
  kSub,
  kMul,
  kDiv,
};

enum class Builtin {
  kCons,
  kCar,
  kCdr,
};

struct ElementaryType {
  enum class Kind {
    kInt,
  };
  Kind kind;
};

std::string ToString(enum ElementaryType::Kind kind);
std::string ToString(enum Op op);
std::string ToString(enum Builtin builtin);

struct ListType;
struct TupleType;

using Type = std::variant<ElementaryType, ListType, TupleType>;

struct ListType {
  std::unique_ptr<Type> type;
};

struct TupleType {
  std::vector<std::unique_ptr<Type>> types;
};

struct Arg {
  std::size_t ident_code;
  Type type;
};

struct FuncType {
  std::vector<Arg> args;
  Type result;
};

struct IfStatement;

struct Call;

struct Ident {
  std::size_t code;
};

struct IntConst {
  std::int64_t value;
};

using Const = std::variant<IntConst>;

struct BinaryExpr;
struct ListExpr;
struct TupleExpr;

using Expr = std::variant<Call, Ident, Const, BinaryExpr, ListExpr, TupleExpr>;

struct ListExpr {
  std::vector<std::unique_ptr<Expr>> elements;
};

struct TupleExpr {
  std::vector<std::unique_ptr<Expr>> elements;
};

struct BinaryExpr {
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  Op op;
};


using Callee = std::variant<Ident, Builtin>;

struct Call {
  Callee callee;
  std::vector<Expr> args;
};

using Statement = std::variant<IfStatement, Expr>;

struct OrExpr;

struct NullExpr {
  Expr inner;
};

using BoolExpr = std::variant<OrExpr, Expr, NullExpr>;

struct OrExpr {
  std::unique_ptr<BoolExpr> lhs;
  std::unique_ptr<BoolExpr> rhs;
};

struct IfStatement {
  BoolExpr condition;
  Expr then_expr;
  Expr else_expr;
};

struct FuncBody {
  Statement statement;
};

struct Func {
  std::size_t ident_code;
  FuncType type;
  FuncBody body;
};

using Program = std::vector<Func>;

}  // namespace stewkk::lab11
