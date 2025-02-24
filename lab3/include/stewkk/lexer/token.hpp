#pragma once

#include <cstddef>

#include <stewkk/lexer/domain.hpp>

namespace stewkk::lexer {

struct Position {
  std::size_t line;
  std::size_t column;

  bool operator==(const Position&) const = default;
};

struct Token {
  DomainType domain;
  Position position;
  std::string text;

  bool operator==(const Token&) const = default;
};

}  // namespace stewkk::lexer
