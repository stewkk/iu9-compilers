#include <stewkk/lexer/lexer.hpp>

#include <utf8.h>
#include <mach7/match.hpp>
#include <mach7/patterns/n+k.hpp>          // Generalized n+k patterns
#include <mach7/patterns/predicate.hpp>    // Support for predicate patterns
#include <mach7/patterns/primitive.hpp>    // Wildcard, variable and value patterns

namespace stewkk::lexer {

namespace {

Position NextPosition(Position pos, char32_t code_point) {
  return std::iswspace(code_point) ? Position{.line = pos.line + 1, .column = pos.column}
                                   : Position{.line = pos.line, .column = pos.column + 1};
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

}  // namespace

TokenizerOutput Tokenize(
    char32_t code_point, const TokenizerState& state) {
  return std::visit([&code_point](const auto& state) { return HandleState(code_point, state); },
                    state);
}

TokenizerStringOutput Tokenize(std::string s, const TokenizerState& state) {
  return s.empty() ? std::make_tuple(state, immer::flex_vector<Token>{}, immer::flex_vector<Message>{})
                   : [&state, &s] {
                       // TODO: utf8
                       const auto [next_state, token, message] = Tokenize(s.front(), state);
                       const auto [res_state, tokens, messages] = Tokenize(s.substr(1, s.size()-1), next_state);
                       const auto res_tokens = token.has_value() ? tokens.push_front(token.value()) : tokens;
                       const auto res_messages = message.has_value() ? messages.push_front(message.value()) : messages;
                       return std::make_tuple(res_state, res_tokens, res_messages);
                     }();
}

}  // namespace stewkk::lexer
