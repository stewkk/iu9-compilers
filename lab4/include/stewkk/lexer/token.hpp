#pragma once

#include <cstdint>
#include <variant>
#include <string>

#include <strong_type/strong_type.hpp>

namespace stewkk::lexer {

struct Position {
  std::size_t line;
  std::size_t column;

  bool operator==(const Position&) const = default;
};

struct Coords {
  Position start;
  Position end;

  bool operator==(const Coords&) const = default;
};

template <typename Attr>
struct TokenData {
  Coords coords;
  Attr attr;

  bool operator==(const TokenData&) const = default;
};

template <typename Tag, typename Attr> using TokenType = strong::type<TokenData<Attr>, Tag, strong::equality>;

using StringLiteralToken = TokenType<struct string_token_, std::string>;
using IntegerToken = TokenType<struct integer_token_, std::int64_t>;
using IdentToken = TokenType<struct ident_token_, std::size_t>;
using EofToken = TokenType<struct eof_token_, char>;

using Token = std::variant<StringLiteralToken, IntegerToken, IdentToken, EofToken>;

std::string GetName(const StringLiteralToken&);
std::string GetName(const IntegerToken&);
std::string GetName(const IdentToken&);
std::string GetName(const EofToken&);

std::string ToString(Token token);

}  // namespace stewkk::lexer
