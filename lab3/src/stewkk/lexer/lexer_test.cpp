#include <gmock/gmock.h>

#include <array>
#include <optional>
#include <string_view>
#include <utility>
#include <iterator>

#include <re2/re2.h>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <stewkk/lexer/domain.hpp>

using ::testing::Eq;
using ::testing::Optional;

using std::string_literals::operator""s;
using std::string_view_literals::operator""sv;

namespace stewkk::lexer {

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

Position NextPosition(Position pos, const std::string& token) {
  auto it = token.begin();
  while (true) {
    auto prev = it;
    it = std::find(it, token.end(), '\n');;
    if (it == token.end()) {
      pos.column += it - prev;
      return pos;
    }
    pos.line += 1;
    pos.column = 1;
    it++;
  }
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

class LexerError : public std::exception {
  public:
    explicit LexerError(Position pos) : pos_(std::move(pos)) {}
    Position GetPosition() const {
      return pos_;
    }
  private:
    Position pos_;
};

class Lexer {
  public:
    explicit Lexer(std::string text);

    struct Iterator {
      using difference_type = std::ptrdiff_t;
      using value_type = Token;

      Iterator() : token_(), rest_() {}
      Iterator(std::string_view rest, Matcher m)
          : token_(), rest_(std::move(rest)), m_(std::move(m)), pos_(Position{1, 1}) {
        if (rest_.empty()) {
          return;
        }
        auto match = m_.NextMatch(rest_, pos_);
        if (!match.has_value()) {
          throw LexerError{std::move(pos_)};
        }
        token_ = match.value().match;
        rest_ = match.value().rest;
        pos_ = NextPosition(pos_, token_.text);
      }

      Token operator*() const {
        return token_;
      }

      Iterator& operator++() {
        if (rest_.empty()) {
          token_ = Token{};
          return *this;
        }
        auto match = m_.NextMatch(rest_, pos_);
        if (!match.has_value()) {
          throw LexerError{std::move(pos_)};
        }
        token_ = match.value().match;
        rest_ = match.value().rest;
        pos_ = NextPosition(pos_, token_.text);
        return *this;
      }

      Iterator operator++(int) {
        auto tmp = *this;
        ++*this;
        return tmp;
      }

      bool operator==(const Iterator& other) const {
        return rest_ == other.rest_;
      }

      Token token_;
      Position pos_;
      std::string_view rest_;
      Matcher m_;
    };
    static_assert(std::forward_iterator<Iterator>);

    Iterator begin() { return Iterator(text_, m_); }
    Iterator end() { return Iterator(); }

  private:
    std::string text_;
    Matcher m_;
};

Lexer::Lexer(std::string text) : text_(text), m_() {}

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
