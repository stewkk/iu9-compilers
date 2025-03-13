#include <gmock/gmock.h>

#include <utf8.h>

#include <stewkk/lexer/domain.hpp>
#include <stewkk/lexer/token.hpp>
#include <stewkk/lexer/lexer.hpp>

using ::testing::Eq;

namespace stewkk::lexer {

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
