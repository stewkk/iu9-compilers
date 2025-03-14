#include <stewkk/lexer/lexer.hpp>

#include <stdexcept>
#include <functional>

#include <utf8.h>
#include <mach7/type_switchN-patterns.hpp> // Support for N-ary Match statement on patterns
#include <mach7/patterns/predicate.hpp>    // Support for predicate patterns

namespace stewkk::lexer {

namespace {

Position NextPosition(Position pos, char32_t code_point) {
  return code_point == '\n' ? Position{.line = pos.line + 1, .column = pos.column}
                            : Position{.line = pos.line, .column = pos.column + 1};
}

bool IsSpace(char32_t c) { return std::iswspace(c); }

bool IsDigit(char32_t c) { return std::iswdigit(c); }

bool IsAlpha(char32_t c) { return std::iswalpha(c); };

bool Contains(std::u32string s, char32_t c) {
  return std::find(std::begin(s), std::end(s), c) != std::end(s);
}

template <typename Predicate>
concept CharPredicate =
requires(Predicate p, char32_t c) {
    { p(c) } -> std::same_as<bool>;
};

template <CharPredicate Predicate>
bool Any(char32_t c, Predicate predicate) {
  return predicate(c);
}

template <CharPredicate Predicate, CharPredicate... Predicates>
bool Any(char32_t c, Predicate predicate, Predicates... predicates) {
  return predicate(c) || Any(c, predicates...);
}

TokenizerOutput HandleState(
    char32_t code_point, const Whitespace& state) {
  auto IsSpecial = std::bind(Contains, U"_$@", std::placeholders::_1);
  auto IsIdent = std::bind(Any<decltype(IsSpecial), decltype(IsAlpha)>, std::placeholders::_1,
                           IsSpecial, IsAlpha);
  auto IsIdentRef = std::cref(IsIdent);
  const auto token_start = state.value_of().token_end;
  const auto token_end = NextPosition(state.value_of().token_end, code_point);
  const auto token_prefix = state.value_of().token_prefix;

  Match(code_point) {
    Case(IsSpace) return std::make_tuple(
        Whitespace(TokenizerStateData{
            .token_prefix = immer::flex_vector<char32_t>{},
            .token_start = token_start,
            .token_end = token_end,
        }),
        std::nullopt, std::nullopt);
    Case(IsDigit) return std::make_tuple(
        Number(TokenizerStateData{
            .token_prefix = token_prefix.push_back(code_point),
            .token_start = token_start,
            .token_end = token_end,
        }),
        std::nullopt, std::nullopt);
    Case('"') return std::make_tuple(
        Str(TokenizerStateData{
            .token_prefix = immer::flex_vector<char32_t>{},
            .token_start = token_start,
            .token_end = token_end,
        }),
        std::nullopt, std::nullopt);
    Case(IsIdentRef) return std::make_tuple(
        Ident(TokenizerStateData{
            .token_prefix = token_prefix.push_back(code_point),
            .token_start = token_start,
            .token_end = token_end,
        }),
        std::nullopt, std::nullopt);
  }
  EndMatch throw std::logic_error{"unreachable"};
}

TokenizerOutput HandleState(
    char32_t code_point, const Str& state) {
  Match(code_point) {
    Case('\\') return std::make_tuple(
        Escape(TokenizerStateData{
            .token_prefix = state.value_of().token_prefix,
            .token_start = state.value_of().token_start,
            .token_end = NextPosition(state.value_of().token_end, code_point),
        }),
        std::nullopt, std::nullopt);
    Case('"') return std::make_tuple(
        Whitespace(TokenizerStateData{
            .token_prefix = immer::flex_vector<char32_t>{},
            .token_start = state.value_of().token_end,
            .token_end = NextPosition(state.value_of().token_end, code_point),
        }),
        std::nullopt /* TODO: token here */, std::nullopt);
  }
  EndMatch throw std::logic_error{"unreachable"};
}

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
  return s.empty() ? std::make_tuple(state, Tokens{}, Messages{})
                   : [&state, &s] {
                       // TODO: utf8
                       const auto [next_state, token, message] = Tokenize(s.front(), state);
                       const auto [res_state, tokens, messages] = Tokenize(s.substr(1, s.size()-1), next_state);
                       // TODO: monadic optional
                       const auto res_tokens = token.has_value() ? tokens.push_front(token.value()) : tokens;
                       const auto res_messages = message.has_value() ? messages.push_front(message.value()) : messages;
                       return std::make_tuple(res_state, res_tokens, res_messages);
                     }();
}

}  // namespace stewkk::lexer
