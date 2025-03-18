#include <iostream>

#include <utf8.h>
#include <optional>

#include <stewkk/lexer/lexer.hpp>
#include <stewkk/lexer/token.hpp>

int main() {
  stewkk::lexer::TokenizerState state{stewkk::lexer::Whitespace{stewkk::lexer::TokenizerStateData{}}};
  std::optional<stewkk::lexer::Token> token;
  std::optional<stewkk::lexer::Message> message;
  auto b = std::istreambuf_iterator<char>(std::cin);
  auto e = std::istreambuf_iterator<char>();

  while (true) {
    const char32_t next = utf8::next(b, e);
    std::tie(state, token, message) = stewkk::lexer::Tokenize(next, state);

    if (token.has_value()) {
      std::cout << ToString(token.value()) << std::endl;
    }
    if (message.has_value()) {
      std::cout << message.value() << std::endl;
    }
    if (b == e) {
      break;
    }
  }

  std::tie(state, token, message) = stewkk::lexer::Tokenize(stewkk::lexer::kEofMarker, state);
  if (token.has_value()) {
    std::cout << ToString(token.value()) << std::endl;
  }
  if (message.has_value()) {
    std::cout << message.value() << std::endl;
  }

  return 0;
}
