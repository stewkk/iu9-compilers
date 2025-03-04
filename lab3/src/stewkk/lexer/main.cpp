#include <iostream>
#include <string>

#include <stewkk/lexer/lexer.hpp>

int main() {
  std::string input((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
  stewkk::lexer::OutputTokens(input);
  return 0;
}
