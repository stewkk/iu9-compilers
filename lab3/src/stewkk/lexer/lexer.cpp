#include <stewkk/lexer/lexer.hpp>

#include <algorithm>
#include <format>
#include <iostream>

#include <stewkk/lexer/error.hpp>

namespace stewkk::lexer {

using Iterator = Lexer::Iterator;

namespace {

Position NextPosition(Position pos, const std::string& token) {
  auto it = token.begin();
  while (true) {
    auto prev = it;
    it = std::find(it, token.end(), '\n');
    ;
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

Lexer::Lexer(std::string text) : text_(text), m_() {}

Iterator::Iterator() : token_(), rest_() {}

Iterator::Iterator(std::string_view rest, Matcher m)
    : token_(), rest_(std::move(rest)), m_(std::move(m)), pos_(Position{1, 1}) {
  if (rest_.empty()) {
    return;
  }
  auto match = m_.NextMatch(rest_, pos_);
  if (!match.has_value()) {
    throw LexerError{std::move(pos_)};
  }
  token_ = std::move(match).value().match;
  rest_ = std::move(match).value().rest;
  pos_ = NextPosition(pos_, token_.text);
}

Token Iterator::operator*() const { return token_; }

Iterator& Iterator::operator++() {
  if (rest_.empty()) {
    token_ = Token{};
    return *this;
  }

  auto match = m_.NextMatch(rest_, pos_);
  if (!match.has_value()) {
    throw LexerError{std::move(pos_)};
  }

  token_ = std::move(match).value().match;
  rest_ = std::move(match).value().rest;
  pos_ = NextPosition(pos_, token_.text);
  return *this;
}

Iterator Iterator::operator++(int) {
  auto tmp = *this;
  ++*this;
  return tmp;
}

bool Iterator::operator==(const Iterator& other) const { return rest_ == other.rest_; }

const Iterator Lexer::begin() { return Iterator(text_, m_); }

const Iterator Lexer::end() { return Iterator(); }

void OutputTokens(const std::string& text) {
  Lexer l(text);
  try {
    for (const auto token : l) {
      std::cout << std::format("{} ({},{}): {}", ToString(token.domain), token.position.line,
                               token.position.column, token.text)
                << std::endl;
    }
  } catch (const LexerError& e) {
    std::cerr << std::format("syntax error ({},{})", e.GetPosition().line, e.GetPosition().column)
              << std::endl;
  }
}

}  // namespace stewkk::lexer
