#include <gmock/gmock.h>

#include <string_view>

#include <stewkk/lexer/domain.hpp>
#include <stewkk/lexer/token.hpp>
#include <stewkk/lexer/matcher.hpp>
#include <stewkk/lexer/error.hpp>
#include <stewkk/lexer/lexer.hpp>

using ::testing::Eq;
using ::testing::Optional;

using std::string_literals::operator""s;
using std::string_view_literals::operator""sv;

namespace stewkk::lexer {

TEST(MatcherTest, MatchesOpeningTag) {
  auto text = "<div> </div>"sv;
  Matcher m;

  auto match = m.NextMatch(text, Position{1, 1});

  ASSERT_THAT(match, Optional(Match{
      Token{
        .domain = DomainType::kOpeningTag,
        .position = Position{
          .line = 1,
          .column = 1,
        },
        .text = "<div>"s,
      },
      " </div>"sv}));
}

TEST(MatcherTest, MatchesClosingTag) {
  auto text = "</div>"sv;
  Matcher m;

  auto match = m.NextMatch(text, Position{1, 1});

  ASSERT_THAT(match, Optional(Match{
      Token{
        .domain = DomainType::kClosingTag,
        .position = Position{
          .line = 1,
          .column = 1,
        },
        .text = "</div>"s,
      },
      ""sv}));
}

TEST(MatcherTest, MatchesSpaces) {
  auto text = "  \n <div>"sv;
  Matcher m;

  auto match = m.NextMatch(text, Position{1, 1});

  ASSERT_THAT(match, Optional(Match{
      Token{
        .domain = DomainType::kWhitespace,
        .position = Position{
          .line = 1,
          .column = 1,
        },
        .text = "  \n "s,
      },
      "<div>"sv}));
}

TEST(MatcherTest, MatchesLt) {
  auto text = "&lt;  \n <div>"sv;
  Matcher m;

  auto match = m.NextMatch(text, Position{1, 1});

  ASSERT_THAT(match, Optional(Match{
      Token{
        .domain = DomainType::kLt,
        .position = Position{
          .line = 1,
          .column = 1,
        },
        .text = "&lt;"s,
      },
      "  \n <div>"sv}));
}

TEST(MatcherTest, MatchesSymbol) {
  auto text = "lt;  \n <div>"sv;
  Matcher m;

  auto match = m.NextMatch(text, Position{1, 1});

  ASSERT_THAT(match, Optional(Match{
      Token{
        .domain = DomainType::kSymbol,
        .position = Position{
          .line = 1,
          .column = 1,
        },
        .text = "l"s,
      },
      "t;  \n <div>"sv}));
}

TEST(LexerTest, IteratesOverTokens) {
  Lexer l("<div> ");
  auto it = l.begin();

  ASSERT_THAT(*it, Eq(
      Token{
        .domain = DomainType::kOpeningTag,
        .position = Position{
          .line = 1,
          .column = 1,
        },
        .text = "<div>"s,
      }));

  it++;

  ASSERT_THAT(*it, Eq(
      Token{
        .domain = DomainType::kWhitespace,
        .position = Position{
          .line = 1,
          .column = 6,
        },
        .text = " "s,
      }));

  it++;

  ASSERT_THAT(it, Eq(l.end()));
}

TEST(LexerTest, HandlesEOL) {
  Lexer l("<div> \n </div>");
  auto it = l.begin();
  it++;
  it++;

  ASSERT_THAT(*it, Eq(
      Token{
        .domain = DomainType::kClosingTag,
        .position = Position{
          .line = 2,
          .column = 2,
        },
        .text = "</div>"s,
      }));
}

TEST(LexerTest, HandlesTwoEOL) {
  Lexer l("<div> \n \n</div>");
  auto it = l.begin();
  it++;
  it++;

  ASSERT_THAT(*it, Eq(
      Token{
        .domain = DomainType::kClosingTag,
        .position = Position{
          .line = 3,
          .column = 1,
        },
        .text = "</div>"s,
      }));
}

}  // namespace stewkk::lexer
