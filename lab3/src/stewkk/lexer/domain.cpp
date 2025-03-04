#include <stewkk/lexer/domain.hpp>

namespace stewkk::lexer {

std::string ToString(DomainType type) {
  switch (type) {
    case DomainType::kOpeningTag:
      return "OTAG";
    case DomainType::kClosingTag:
      return "CTAG";
    case DomainType::kWhitespace:
      return "WS";
    case DomainType::kLt:
      return "LT";
    case DomainType::kGt:
      return "GT";
    case DomainType::kAmp:
      return "AMP";
    case DomainType::kSymbol:
      return "SYM";
  }
  throw "unreachable";
}

}  // namespace stewkk::lexer
