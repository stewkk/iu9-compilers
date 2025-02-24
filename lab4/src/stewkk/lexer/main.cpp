#include <iostream>

#include <stewkk/lexer/lexer.hpp>

int main() {
  std::string line;

  stewkk::lexer::TokenizerState state{stewkk::lexer::Whitespace{stewkk::lexer::TokenizerStateData{}}};
  stewkk::lexer::Tokens tokens;
  stewkk::lexer::Messages messages;

  while (std::getline(std::cin, line)) {
    line.push_back('\n');
    std::tie(state, tokens, messages) = stewkk::lexer::Tokenize(
        line, state);

    for (const auto& token : tokens) {
      std::cout << stewkk::lexer::ToString(token) << std::endl;
    }
    for (const auto& message : messages) {
      std::cout << message << std::endl;
    }
  }

  auto [_, token, message] = stewkk::lexer::Tokenize(stewkk::lexer::kEofMarker, state);
  if (token.has_value()) {
    std::cout << ToString(token.value()) << std::endl;
  }
  if (message.has_value()) {
    std::cout << message.value() << std::endl;
  }

  return 0;
}
