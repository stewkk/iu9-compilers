#pragma once

#include <string>
#include <string_view>
#include <iterator>

#include <stewkk/lexer/token.hpp>
#include <stewkk/lexer/matcher.hpp>

namespace stewkk::lexer {

class Lexer {
  public:
    explicit Lexer(std::string text);

    struct Iterator {
      using difference_type = std::ptrdiff_t;
      using value_type = Token;

      Iterator();
      Iterator(std::string_view rest, Matcher m);

      Token operator*() const;

      Iterator& operator++();

      Iterator operator++(int);

      bool operator==(const Iterator& other) const;

      Token token_;
      Position pos_;
      std::string_view rest_;
      Matcher m_;
    };
    static_assert(std::forward_iterator<Iterator>);

    const Iterator begin();
    const Iterator end();

  private:
    std::string text_;
    Matcher m_;
};

void OutputTokens(const std::string& text);

}  // namespace stewkk::lexer
