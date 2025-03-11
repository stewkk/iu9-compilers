#include <gmock/gmock.h>

#include <tuple>

#include <immer/vector.hpp>

using ::testing::Eq;

namespace stewkk::lexer {

enum class DomainType { kInteger };

struct Token {
    DomainType type;

    bool operator==(const Token&) const = default;
};

struct Message {
    bool operator==(const Message&) const = default;
};

enum class TokenizerStateType { kWhitespace, kStr, kEscape, kNumber, kIdent };

struct TokenizerState {
  TokenizerStateType type;
  immer::vector<char32_t> token_prefix;

  bool operator==(const TokenizerState&) const = default;
};

std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>> Tokenize(
    char32_t code_point, TokenizerState state) {
  return std::make_tuple(TokenizerState{}, std::nullopt, std::nullopt);
}

TEST(LexerTest, Zero) {
  ASSERT_THAT(Tokenize('0', TokenizerState{}), Eq(std::make_tuple(
                                                   TokenizerState{
                                                       .type = TokenizerStateType::kNumber,
                                                       .token_prefix = immer::vector<char32_t>{'0'},
                                                   },
                                                   std::nullopt, std::nullopt)));
}

}  // namespace stewkk::lexer
