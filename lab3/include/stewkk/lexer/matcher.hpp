#pragma once

#include <string_view>
#include <optional>

#include <stewkk/lexer/token.hpp>

namespace stewkk::lexer {

struct Match {
  Token match;
  std::string_view rest;

  bool operator==(const Match&) const = default;
};

class Matcher {
public:
  std::optional<Match> NextMatch(std::string_view text, Position pos);
};

}  // namespace stewkk::lexer
