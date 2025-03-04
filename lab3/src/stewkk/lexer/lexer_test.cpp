#include <gmock/gmock.h>

#include <array>
#include <optional>
#include <string_view>
#include <utility>

#include <re2/re2.h>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

using ::testing::Eq;
using ::testing::Optional;

using std::string_literals::operator""s;
using std::string_view_literals::operator""sv;

namespace stewkk::lexer {

// TODO: интерфейс на итераторах

enum class DomainType { kOpeningTag, kClosingTag };

struct Position {
  std::size_t line;
  std::size_t column;

  bool operator==(const Position&) const = default;
};

struct Token {
  DomainType domain;
  Position position;
  std::string text;

  bool operator==(const Token&) const = default;
};

struct Match {
  Token match;
  std::string_view rest;

  bool operator==(const Match&) const = default;
};

class Matcher {
public:
  std::optional<Match> NextMatch(std::string_view text, Position pos);
};

struct Domain {
  std::string pattern;
  DomainType type;
};

static const std::array<Domain, 2> kDomainPatterns{{
    {R"((<(?:\w|\d)+>))", DomainType::kOpeningTag},
    {R"((<\/(?:\w|\d)+>))", DomainType::kClosingTag},
}};

namespace {

struct TextMatch {
  std::string_view rest;
  std::string content;
};

std::optional<TextMatch> TryMatch(std::string_view text, const std::string& pattern) {
  std::string content;
  if (!RE2::Consume(&text, pattern, &content)) {
    return std::nullopt;
  }

  return TextMatch{
      .rest = text,
      .content = content,
  };
}

}  // namespace

std::optional<Match> Matcher::NextMatch(std::string_view text, Position pos) {
  const auto matches = kDomainPatterns
                       // Find matches in text
                       | ranges::views::transform([&text](const auto& domain) {
                           return TryMatch(text, domain.pattern);
                         });

  auto [match, domain] = ranges::max(ranges::views::zip(matches, kDomainPatterns), std::less(),
                                     [](const auto& match_with_domain) {
                                       const auto& [match, _] = match_with_domain;
                                       if (!match.has_value()) {
                                         return 0ul;
                                       }
                                       const auto& content = match.value().content;
                                       return content.size();
                                     });

  if (!match.has_value()) {
    return std::nullopt;
  }

  return Match{
      .match = Token{
        .domain = domain.type,
        .position = pos,
        .text = std::move(match).value().content,
      },
      .rest = std::move(match).value().rest,
    };
}

TEST(MatcherTest, MatchesOpeningTag) {
  auto text = "<div> </div>"sv;
  Matcher l;

  auto match = l.NextMatch(text, Position{1, 1});

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
  Matcher l;

  auto match = l.NextMatch(text, Position{1, 1});

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

}  // namespace stewkk::lexer
