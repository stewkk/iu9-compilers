#include <stewkk/lexer/token.hpp>

#include <mach7/type_switchN-patterns.hpp> // Support for N-ary Match statement on patterns
#include <mach7/patterns/constructor.hpp>  // Support for constructor patterns

namespace stewkk::lexer {

std::string GetName(const StringLiteralToken&) {
    return "STRING";
}

std::string GetName(const IntegerToken&) {
    return "INTEGER";
}

std::string GetName(const IdentToken&) {
    return "IDENT";
}

std::string GetName(const EofToken&) {
    return "EOF";
}

std::string ToString(Token token_variant) {
  return std::visit(
      [](const auto& token) {
        const auto& token_value = token.value_of();
        return std::format("{} ({}:{})-({}:{}): {}", GetName(token), token_value.coords.start.line+1,
                           token_value.coords.start.column+1, token_value.coords.end.line+1,
                           token_value.coords.end.column+1, token_value.attr);
      },
      token_variant);
}


}  // namespace stewkk::lexer
