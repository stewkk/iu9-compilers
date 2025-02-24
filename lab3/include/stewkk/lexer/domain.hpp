#pragma once

#include <array>
#include <string>

namespace stewkk::lexer {

enum class DomainType { kOpeningTag, kClosingTag, kWhitespace, kLt, kGt, kAmp, kSymbol };

std::string ToString(DomainType type);

struct Domain {
  std::string pattern;
  DomainType type;
};

static const std::array<Domain, 7> kDomainPatterns{{
    {R"((<(?:\w|\d)+>))", DomainType::kOpeningTag},
    {R"((<\/(?:\w|\d)+>))", DomainType::kClosingTag},
    {R"((\s+))", DomainType::kWhitespace},
    {R"((&lt;))", DomainType::kLt},
    {R"((&gt;))", DomainType::kGt},
    {R"((&amp;))", DomainType::kAmp},
    {R"(([^&<>]))", DomainType::kSymbol},
}};

}  // namespace stewkk::lexer
