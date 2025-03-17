#include <stewkk/lexer/token.hpp>

#include <mach7/type_switchN-patterns.hpp> // Support for N-ary Match statement on patterns
#include <mach7/patterns/constructor.hpp>  // Support for constructor patterns

namespace stewkk::lexer {

std::string GetName(Token token) {
  Match(token) {
      Case(mch::C<StringLiteralToken>()) return "STRING";
      Case(mch::C<IntegerToken>()) return "INTEGER";
      Case(mch::C<IdentToken>()) return "IDENT";
  }
  EndMatch throw std::logic_error{"unreachable"};
}

std::string ToString(Token token_variant) {
  return std::visit(
      [](const auto& token) {
        const auto& token_value = token.value_of();
        return std::format("{} ({}:{})-({}:{}): {}", GetName(token), token_value.coords.start.line,
                           token_value.coords.start.column, token_value.coords.end.line,
                           token_value.coords.end.column, token_value.attr);
      },
      token_variant);
}


}  // namespace stewkk::lexer
