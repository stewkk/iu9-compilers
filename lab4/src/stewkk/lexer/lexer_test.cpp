#include <gmock/gmock.h>

#include <array>
#include <tuple>
#include <cwctype>

#include <immer/vector.hpp>
#include <mach7/type_switchN-patterns.hpp> // Support for N-ary Match statement on patterns
#include <mach7/patterns/address.hpp>      // Address and dereference combinators
#include <mach7/patterns/bindings.hpp>     // Mach7 support for bindings on arbitrary UDT
#include <mach7/patterns/constructor.hpp>  // Support for constructor patterns
#include <mach7/patterns/equivalence.hpp>  // Equivalence combinator +
#include <mach7/patterns/primitive.hpp>    // Wildcard, variable and value patterns
#include <utf8.h>
#include <strong_type/strong_type.hpp>

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
    return Position{.line = pos.line+1, .column = pos.column};
  }
  return Position{.line = pos.line, .column = pos.column+1};
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

using Whitespace = strong::type<TokenizerStateData, struct whitespace_>;
using Str = strong::type<TokenizerStateData, struct str_>;
using Escape = strong::type<TokenizerStateData, struct escape_>;
using Number = strong::type<TokenizerStateData, struct number_>;
using Ident = strong::type<TokenizerStateData, struct ident_>;
using Eof = strong::type<TokenizerStateData, struct eof_>;

using TokenizerState = std::variant<Whitespace, Str, Escape, Number, Ident, Eof>;

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> HandleWhitespace(
  char32_t code_point, const TokenizerState& state) {
          // return std::make_tuple(
          //     TokenizerState{
          //         .token_prefix = state.token_prefix.push_back(code_point),
          //         .token_start = state.token_start,
          //         .token_end = NextPosition(state.token_end, code_point),
          //     },
          //     std::nullopt, std::nullopt);
}

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> Tokenize(
    char32_t code_point, const TokenizerState& state) {

}

// TEST(LexerTest, Zero) {
//   ASSERT_THAT(Tokenize('0',
//                        TokenizerState{
//                            .type = TokenizerStateType::kWhitespace,
//                            .token_prefix = immer::vector<char32_t>{},
//                            .token_start = Position{0, 0},
//                            .token_end = Position{0, 0},
//                        }),
//               Eq(std::make_tuple(
//                   TokenizerState{
//                       .type = TokenizerStateType::kNumber,
//                       .token_prefix = immer::vector<char32_t>{'0'},
//                       .token_start = Position{0, 0},
//                       .token_end = Position{0, 1},
//                   },
//                   std::nullopt, std::nullopt)));
// }

}  // namespace stewkk::lexer

