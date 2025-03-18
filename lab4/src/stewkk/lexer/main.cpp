#include <iostream>

#include <utf8.h>
#include <optional>

#include <stewkk/lexer/lexer.hpp>
#include <stewkk/lexer/token.hpp>

int main() {
  stewkk::lexer::TokenizerState state{stewkk::lexer::Whitespace{stewkk::lexer::TokenizerStateData{}}};
  immer::flex_vector<stewkk::lexer::Token> tokens;
  std::optional<stewkk::lexer::Message> message;
  auto b = std::istreambuf_iterator<char>(std::cin);
  auto e = std::istreambuf_iterator<char>();

  while (true) {
    const char32_t next = utf8::next(b, e);
    std::tie(state, tokens, message) = stewkk::lexer::TokenizeWithEof(next, state);

    for (auto token : tokens) {
      std::cout << ToString(token) << std::endl;
    }
    if (message.has_value()) {
      std::cout << message.value() << std::endl;
    }
    if (b == e) {
      break;
    }
  }

  std::tie(state, tokens, message)
      = stewkk::lexer::TokenizeWithEof(stewkk::lexer::kEofMarker, state);
  for (auto token : tokens) {
    std::cout << ToString(token) << std::endl;
  }
  if (message.has_value()) {
    std::cout << message.value() << std::endl;
  }

  return 0;
}
