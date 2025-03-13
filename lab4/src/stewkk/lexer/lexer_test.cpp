#include <gmock/gmock.h>

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
