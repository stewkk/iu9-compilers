#include <gmock/gmock.h>

#include <iostream>
#include <variant>

#include <utf8.h>

#include <stewkk/lexer/token.hpp>
#include <stewkk/lexer/lexer.hpp>

using ::testing::Eq;

using std::string_literals::operator""s;

namespace stewkk::lexer {

TokenizerStringOutput PrintState(const TokenizerStringOutput out) {
  const auto [state_variant, tokens, messages] = out;
  std::visit(
      [](const auto& state) {
        const auto& state_value = state.value_of();
        std::cout << GetName(state) << std::endl;
        std::cout << std::format("    prefix: '{}'\n", ToString(state_value.token_prefix));
        std::cout << std::format("    token start: {} {}\n", state_value.token_start.line,
                                 state_value.token_start.column);
        std::cout << std::format("    prev: {} {}\n", state_value.prev.line, state_value.prev.column);
        std::cout << std::format("    current: {} {}\n", state_value.current.line, state_value.current.column);
      },
      state_variant);
  for (const auto& token : tokens) {
    std::cout << "    " << ToString(token) << std::endl;
  }
  for (const auto& message : messages) {
    std::cout << "    " << message << std::endl;
  }
  return out;
}

TokenizerState GetStartState() {
  return Whitespace(TokenizerStateData{
      .token_prefix = immer::flex_vector<char32_t>{},
      .token_start = Position{0, 0},
      .prev = Position{0, 0},
      .current = Position{0, 0},
      .ident_to_index = immer::map<std::string, std::size_t>{},
      .index_to_ident = immer::flex_vector<std::string>{},
  });
}

TEST(LexerTest, TokenizeSymbol) {
  ASSERT_THAT(Tokenize('0', GetStartState()),
              Eq(std::make_tuple(TokenizerState(Number(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{'0'},
                                     .token_start = Position{0, 0},
                                     .prev = Position{0, 0},
                                     .current = Position{0, 1},
                                 })),
                                 std::nullopt, std::nullopt)));
}

TEST(LexerTest, TokenizeString) {
  ASSERT_THAT(Tokenize("  0"s, GetStartState()),
              Eq(std::make_tuple(TokenizerState(Number(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{'0'},
                                     .token_start = Position{0, 2},
                                     .prev = Position{0, 2},
                                     .current = Position{0, 3},
                                 })),
                                 Tokens{}, Messages{})));
}

TEST(LexerTest, TokenizeQuote) {
  ASSERT_THAT(Tokenize("  \""s, GetStartState()),
              Eq(std::make_tuple(TokenizerState(Str(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{},
                                     .token_start = Position{0, 2},
                                     .prev = Position{0, 2},
                                     .current = Position{0, 3},
                                 })),
                                 Tokens{}, Messages{})));
}

TEST(LexerTest, TokenizeIdent) {
  ASSERT_THAT(Tokenize("  Y"s, GetStartState()),
              Eq(std::make_tuple(TokenizerState(Ident(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{'Y'},
                                     .token_start = Position{0, 2},
                                     .prev = Position{0, 2},
                                     .current = Position{0, 3},
                                 })),
                                 Tokens{}, Messages{})));
}

TEST(LexerTest, TokenizeFullString) {
  ASSERT_THAT(
      Tokenize("  \"oa\\to \\\" a\" "s, GetStartState()),
      Eq(std::make_tuple(TokenizerState(Whitespace(TokenizerStateData{
                             .token_prefix = immer::flex_vector<char32_t>{},
                             .token_start = Position{0, 14},
                             .prev = Position{0, 14},
                             .current = Position{0, 15},
                         })),
                         Tokens{StringLiteralToken(Coords{{0, 2}, {0, 13}}, "oa\to \" a")}, Messages{})));
}

TEST(LexerTest, TokenizeInteger) {
  ASSERT_THAT(Tokenize("  123_999 "s, GetStartState()),
              Eq(std::make_tuple(TokenizerState(Whitespace(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{},
                                     .token_start = Position{0, 9},
                                     .prev = Position{0, 9},
                                     .current = Position{0, 10},
                                 })),
                                 Tokens{IntegerToken(Coords{{0, 2}, {0, 8}}, 123999)}, Messages{})));
}

TEST(LexerTest, TokenizeIdentFull) {
  ASSERT_THAT(
      Tokenize("  Y1@_123 "s, GetStartState()),
      Eq(std::make_tuple(TokenizerState(Whitespace(TokenizerStateData{
                             .token_prefix = immer::flex_vector<char32_t>{},
                             .token_start = Position{0, 9},
                             .prev = Position{0, 9},
                             .current = Position{0, 10},
                             .ident_to_index = immer::map<std::string, std::size_t>{{"Y1@_123", 0}},
                             .index_to_ident = immer::flex_vector<std::string>{{"Y1@_123"}},
                         })),
                         Tokens{IdentToken(Coords{{0, 2}, {0, 8}}, 0u)}, Messages{})));
}

TEST(LexerTest, TokenizeError) {
  ASSERT_THAT(Tokenize("  * "s, GetStartState()),
              Eq(std::make_tuple(TokenizerState(Whitespace(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{},
                                     .token_start = Position{0, 0},
                                     .prev = Position{0, 3},
                                     .current = Position{0, 4},
                                 })),
                                 Tokens{},
                                 Messages{
                                     "Unknown symbol at (1:3): *",
                                 })));
}

TEST(LexerTest, TokenizeUnicodeString) {
  ASSERT_THAT(
      Tokenize("\"я русский!\""s, GetStartState()),
      Eq(std::make_tuple(TokenizerState(Whitespace(TokenizerStateData{
                             .token_prefix = immer::flex_vector<char32_t>{},
                             .token_start = Position{0, 12},
                             .prev = Position{0, 11},
                             .current = Position{0, 12},
                         })),
                         Tokens{StringLiteralToken(Coords{{0, 0}, {0, 11}}, "я русский!")}, Messages{})));
}

TEST(LexerTest, TokenizeEOF) {
  const auto [state, tokens, messages] = Tokenize("123"s, GetStartState());
  ASSERT_THAT(Tokenize(kEofMarker, state),
              Eq(std::make_tuple(TokenizerState(Eof(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{},
                                     .token_start = Position{0, 3},
                                     .prev = Position{0, 3},
                                     .current = Position{0, 4},
                                 })),
                                 Token{IntegerToken(Coords{{0, 0}, {0, 2}}, 123)}, std::nullopt)));
}

}  // namespace stewkk::lexer
