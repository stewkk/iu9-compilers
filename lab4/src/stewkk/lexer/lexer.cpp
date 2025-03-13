#include <stewkk/lexer/lexer.hpp>

#include <mach7/patterns/address.hpp>
#include <mach7/patterns/bindings.hpp>
#include <mach7/patterns/constructor.hpp>
#include <mach7/patterns/equivalence.hpp>
#include <mach7/patterns/primitive.hpp>
#include <mach7/type_switchN-patterns.hpp>

namespace stewkk::lexer {

namespace {

Position NextPosition(Position pos, char32_t code_point) {
  if (std::iswspace(code_point)) {
    return Position{.line = pos.line + 1, .column = pos.column};
  }
  return Position{.line = pos.line, .column = pos.column + 1};
}

TokenizerOutput HandleState(
    char32_t code_point, const Whitespace& state) {
  return std::make_tuple(Number(TokenizerStateData{
                             .token_prefix = state.value_of().token_prefix.push_back(code_point),
                             .token_start = state.value_of().token_start,
                             .token_end = NextPosition(state.value_of().token_end, code_point),
                         }),
                         std::nullopt, std::nullopt);
}

TokenizerOutput HandleState(
    char32_t code_point, const Str& state) {}

TokenizerOutput HandleState(
    char32_t code_point, const Escape& state) {}

TokenizerOutput HandleState(
    char32_t code_point, const Number& state) {}

TokenizerOutput HandleState(
    char32_t code_point, const Ident& state) {}

TokenizerOutput HandleState(
    char32_t code_point, const Eof& state) {}

TokenizerOutput Tokenize(
    char32_t code_point, const TokenizerState& state) {
  return std::visit([&code_point](const auto& state) { return HandleState(code_point, state); },
                    state);
}

}  // namespace

TokenizerOutput Tokenize(
    char32_t code_point, const TokenizerState& state) {
  return std::visit([&code_point](const auto& state) { return HandleState(code_point, state); },
                    state);
}

}  // namespace stewkk::lexer
