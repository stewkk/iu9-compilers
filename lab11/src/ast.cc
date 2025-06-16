#include "ast.h"

namespace stewkk::lab11 {

std::string ToString(enum ElementaryType::Kind kind) {
  switch (kind) {
      case ElementaryType::Kind::kInt:
          return "int";
  }
  throw std::logic_error{"unreachable"};
}

std::string ToString(enum Op op) {
  switch (op) {
      case Op::kAdd:
          return "+";
      case Op::kSub:
          return "-";
      case Op::kMul:
          return "*";
      case Op::kDiv:
          return "/";
  }
  throw std::logic_error{"unreachable"};
}

std::string ToString(enum Builtin builtin) {
  switch (builtin) {
      case Builtin::kCar:
          return "car";
      case Builtin::kCdr:
          return "cdr";
      case Builtin::kCons:
          return "cons";
  }
  throw std::logic_error{"unreachable"};
}

}  // namespace stewkk::lab11
