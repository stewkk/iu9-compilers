#include <gmock/gmock.h>

#include <string_view>
#include <tuple>

#include <immer/vector.hpp>

using ::testing::Eq;

using std::string_view_literals::operator""sv;

namespace stewkk::lexer {

struct Token {
    bool operator==(const Token&) const = default;
};

struct Message {
    bool operator==(const Message&) const = default;
};

std::tuple<immer::vector<Token>, immer::vector<Message>> Tokenize(std::string_view text) {
    return std::make_tuple(immer::vector<Token>{}, immer::vector<Message>{});
}

TEST(LexerTest, Interface) {
    ASSERT_THAT(Tokenize(""sv), Eq(std::make_tuple(immer::vector<Token>{}, immer::vector<Message>{}))); }

}  // namespace stewkk::lexer
