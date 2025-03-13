#include <gmock/gmock.h>

#include <cwctype>
#include <tuple>
#include <variant>

#include <utf8.h>
#include <immer/vector.hpp>
#include <mach7/patterns/address.hpp>       // Address and dereference combinators
#include <mach7/patterns/bindings.hpp>      // Mach7 support for bindings on arbitrary UDT
#include <mach7/patterns/constructor.hpp>   // Support for constructor patterns
#include <mach7/patterns/equivalence.hpp>   // Equivalence combinator +
#include <mach7/patterns/primitive.hpp>     // Wildcard, variable and value patterns
#include <mach7/type_switchN-patterns.hpp>  // Support for N-ary Match statement on patterns
#include <strong_type/strong_type.hpp>
#include <strong_type/type.hpp>

using ::testing::Eq;

namespace stewkk::lexer {

enum class DomainType { kInteger };

struct Position {
  std::size_t line;
  std::size_t column;

  bool operator==(const Position&) const = default;
};

Position NextPosition(Position pos, char32_t code_point) {
  if (std::iswspace(code_point)) {
    return Position{.line = pos.line + 1, .column = pos.column};
  }
  return Position{.line = pos.line, .column = pos.column + 1};
}

struct Token {
  DomainType type;

  bool operator==(const Token&) const = default;
};

struct Message {
  bool operator==(const Message&) const = default;
};

struct TokenizerStateData {
  immer::vector<char32_t> token_prefix;
  Position token_start;
  Position token_end;

  bool operator==(const TokenizerStateData& other) const = default;
};

template <typename Tag> using StateType = strong::type<TokenizerStateData, Tag, strong::equality>;

using Whitespace = StateType<struct whitespace_>;
using Str = StateType<struct str_>;
using Escape = StateType<struct escape_>;
using Number = StateType<struct number_>;
using Ident = StateType<struct ident_>;
using Eof = StateType<struct eof_>;

using TokenizerState = std::variant<Whitespace, Str, Escape, Number, Ident, Eof>;

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> HandleState(
    char32_t code_point, const Whitespace& state) {
  return std::make_tuple(Number(TokenizerStateData{
                             .token_prefix = state.value_of().token_prefix.push_back(code_point),
                             .token_start = state.value_of().token_start,
                             .token_end = NextPosition(state.value_of().token_end, code_point),
                         }),
                         std::nullopt, std::nullopt);
}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> HandleState(
    char32_t code_point, const Str& state) {}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> HandleState(
    char32_t code_point, const Escape& state) {}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> HandleState(
    char32_t code_point, const Number& state) {}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> HandleState(
    char32_t code_point, const Ident& state) {}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> HandleState(
    char32_t code_point, const Eof& state) {}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> Tokenize(
    char32_t code_point, const TokenizerState& state) {
  return std::visit([&code_point](const auto& state) { return HandleState(code_point, state); },
                    state);
}

TEST(LexerTest, Zero) {
  ASSERT_THAT(Tokenize('0', Whitespace(TokenizerStateData{
                                .token_prefix = immer::vector<char32_t>{},
                                .token_start = Position{0, 0},
                                .token_end = Position{0, 0},
                            })),
              Eq(std::make_tuple(TokenizerState(Number(TokenizerStateData{
                                     .token_prefix = immer::vector<char32_t>{'0'},
                                     .token_start = Position{0, 0},
                                     .token_end = Position{0, 1},
                                 })),
                                 std::nullopt, std::nullopt)));
}

}  // namespace stewkk::lexer
