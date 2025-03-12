#include <gmock/gmock.h>

#include <tuple>
#include <cwctype>
#include <iostream>

#include <immer/vector.hpp>
#include <mach7/type_switchN-patterns.hpp> // Support for N-ary Match statement on patterns
#include <mach7/patterns/address.hpp>      // Address and dereference combinators
#include <mach7/patterns/bindings.hpp>     // Mach7 support for bindings on arbitrary UDT
#include <mach7/patterns/constructor.hpp>  // Support for constructor patterns
#include <mach7/patterns/equivalence.hpp>  // Equivalence combinator +
#include <mach7/patterns/primitive.hpp>    // Wildcard, variable and value patterns
#include <utf8.h>

using ::testing::Eq;

namespace stewkk::lexer {

enum class DomainType { kInteger };

struct Position {
  std::size_t line;
  std::size_t column;

  bool operator==(const Position&) const = default;
};

struct Token {
    DomainType type;

    bool operator==(const Token&) const = default;
};

struct Message {
    bool operator==(const Message&) const = default;
};

enum class TokenizerStateType { kWhitespace, kStr, kEscape, kNumber, kIdent, kEOF, kCount };

using tokenizer_states = std::make_integer_sequence<int, static_cast<int>(TokenizerStateType::kCount)>;

struct TokenizerState {
  TokenizerStateType type;
  immer::vector<char32_t> token_prefix;
  Position token_start;
  Position token_end;

  bool operator==(const TokenizerState&) const = default;
};

template <TokenizerStateType type>
struct CodePointHandler {
  static void handle() = delete;
};

template <>
struct CodePointHandler<TokenizerStateType::kWhitespace> {
  static void handle() {
    std::cout << "kWhitespace" << std::endl;
  };
};

template <class Sequence>
struct ct_map;

template <class T, T Head>
struct ct_map<std::integer_sequence<T, Head>> {
  template <template <T> class F>
  static void call(T t) {
    // TODO: assert?
    return F<Head>::handle();
  }
};

template <class T, T Head, T... Tail>
struct ct_map<std::integer_sequence<T, Head, Tail...>> {
  template <template <T> class F>
  static void call(T t) {
    if (t == Head) {
      return F<Head>::handle();
    }
    return F<Head>::handle();
  }
};

Position NextPosition(Position pos, char32_t code_point) {
  if (std::iswspace(code_point)) {
    return Position{.line = pos.line+1, .column = pos.column};
  }
  return Position{.line = pos.line, .column = pos.column+1};
}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> Whitespace(
    char32_t code_point, TokenizerState state) {
  // TODO
  return std::make_tuple(
      TokenizerState{
          .type = TokenizerStateType::kNumber,
          .token_prefix = state.token_prefix.push_back(code_point),
          .token_start = state.token_start,
          .token_end = NextPosition(state.token_end, code_point),
      },
      std::nullopt, std::nullopt);
}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> Tokenize(
    char32_t code_point, const TokenizerState state) {}

TEST(LexerTest, Zero) {
  ASSERT_THAT(Tokenize('0',
                       TokenizerState{
                           .type = TokenizerStateType::kWhitespace,
                           .token_prefix = immer::vector<char32_t>{},
                           .token_start = Position{0, 0},
                           .token_end = Position{0, 0},
                       }),
              Eq(std::make_tuple(
                  TokenizerState{
                      .type = TokenizerStateType::kNumber,
                      .token_prefix = immer::vector<char32_t>{'0'},
                      .token_start = Position{0, 0},
                      .token_end = Position{0, 1},
                  },
                  std::nullopt, std::nullopt)));
}

}  // namespace stewkk::lexer

