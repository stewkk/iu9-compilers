#include <gmock/gmock.h>

// TODO: remove
#include <iostream>
#include <variant>
#include <utf8.h>

#include <stewkk/lexer/domain.hpp>
#include <stewkk/lexer/token.hpp>
#include <stewkk/lexer/lexer.hpp>

using ::testing::Eq;

using std::string_literals::operator""s;

namespace stewkk::lexer {

TEST(LexerTest, TokenizeSymbol) {
  ASSERT_THAT(Tokenize('0', Whitespace(TokenizerStateData{
                                .token_prefix = immer::flex_vector<char32_t>{},
                                .token_start = Position{0, 0},
                                .token_end = Position{0, 0},
                            })),
              Eq(std::make_tuple(TokenizerState(Number(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{'0'},
                                     .token_start = Position{0, 0},
                                     .token_end = Position{0, 1},
                                 })),
                                 std::nullopt, std::nullopt)));
}

TEST(LexerTest, TokenizeString) {
  const auto [state_variant, tokens, messages] = Tokenize("  0"s, Whitespace(TokenizerStateData{
                       .token_prefix = immer::flex_vector<char32_t>{},
                       .token_start = Position{0, 0},
                       .token_end = Position{0, 0},
                   }));

  const auto state = std::get<Number>(state_variant).value_of();

  std::cout << "prefix: ";
  for (const auto& el : state.token_prefix) {
    std::cout << static_cast<char>(el) << ' ';
  }
  std::cout << std::endl;

  std::cout << std::format("token start: {} {}", state.token_start.line, state.token_start.column) << std::endl;
  std::cout << std::format("token end: {} {}", state.token_end.line, state.token_end.column) << std::endl;

  ASSERT_THAT(Tokenize("  0"s, Whitespace(TokenizerStateData{
                                   .token_prefix = immer::flex_vector<char32_t>{},
                                   .token_start = Position{0, 0},
                                   .token_end = Position{0, 0},
                               })),
              Eq(std::make_tuple(TokenizerState(Number(TokenizerStateData{
                                     .token_prefix = immer::flex_vector<char32_t>{'0'},
                                     .token_start = Position{0, 2},
                                     .token_end = Position{0, 3},
                                 })),
                                 immer::flex_vector<Token>{}, immer::flex_vector<Message>{})));
}

}  // namespace stewkk::lexer
