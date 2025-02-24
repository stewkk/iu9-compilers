#include <stewkk/lexer/matcher.hpp>

#include <re2/re2.h>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

namespace stewkk::lexer {

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
                       | ranges::views::transform(
                           [&text](const auto& domain) { return TryMatch(text, domain.pattern); });

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

}  // namespace stewkk::lexer
