#pragma once

#include <exception>

#include <stewkk/lexer/token.hpp>

namespace stewkk::lexer {

class LexerError : public std::exception {
  public:
    explicit LexerError(Position pos);
    Position GetPosition() const;
  private:
    Position pos_;
};

}  // namespace stewkk::lexer
