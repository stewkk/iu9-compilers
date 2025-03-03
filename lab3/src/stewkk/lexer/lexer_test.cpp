#include <gmock/gmock.h>

#include <string_view>

using ::testing::Eq;

using std::string_view_literals::operator""sv;

namespace stewkk::lexer {

// TODO: интерфейс на итераторах

struct Token {};

struct Match {
  Token token;
  std::string_view rest;
};

class Lexer {
  public:
    Match NextMatch(std::string_view text);
};

Match Lexer::NextMatch(std::string_view text) {}

TEST(LexerTest, MatchesOpeningTag) {
  Lexer l;
  auto text = "<div>"sv;

  auto [token, rest] = l.NextMatch(text);
}

}  // namespace stewkk::lexer
