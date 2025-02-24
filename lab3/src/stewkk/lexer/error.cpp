#include <stewkk/lexer/error.hpp>

namespace stewkk::lexer {

LexerError::LexerError(Position pos) : pos_(std::move(pos)) {}

Position LexerError::GetPosition() const { return pos_; }

}  // namespace stewkk::lexer
