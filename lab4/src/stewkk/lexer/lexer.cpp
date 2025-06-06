#include <stewkk/lexer/lexer.hpp>

#include <stdexcept>
#include <functional>

#include <utf8.h>
#include <mach7/type_switchN-patterns.hpp> // Support for N-ary Match statement on patterns
#include <mach7/patterns/predicate.hpp>    // Support for predicate patterns
#include <mach7/patterns/constructor.hpp>  // Support for constructor patterns

namespace stewkk::lexer {

namespace {

Position NextPosition(Position pos, char32_t code_point) {
  return code_point == '\n' ? Position{.line = pos.line + 1, .column = 0}
                            : Position{.line = pos.line, .column = pos.column + 1};
}

bool IsSpace(char32_t c) { return std::iswspace(c); }

bool IsDigit(char32_t c) { return std::iswdigit(c); }

bool IsAlpha(char32_t c) { return std::iswalpha(c); };

bool Contains(std::u32string s, char32_t c) {
  return std::find(std::begin(s), std::end(s), c) != std::end(s);
}

template <typename Predicate>
concept CharPredicate =
requires(Predicate p, char32_t c) {
    { p(c) } -> std::same_as<bool>;
};

template <CharPredicate... Predicates>
bool Any(char32_t c, Predicates... predicates) {
  return (... || predicates(c));
}

bool IsIdentFirst(char32_t c) {
  return Any(c, std::bind(Contains, U"_$@", std::placeholders::_1), IsAlpha);
}

TokenizerOutput HandleState(
    char32_t code_point, const Whitespace& state) {
  const auto [token_prefix, token_start, prev, current, ident_to_index, index_to_ident]
      = state.value_of();

  Match(code_point) {
    Case(IsSpace) return {
        Whitespace(
            state.value_of().DiscardPrefix().MovePositionBy(code_point)),
        std::nullopt, std::nullopt};
    Case(IsDigit) return {Number(state.value_of()
                                     .AddToPrefix(code_point)
                                     .MovePositionBy(code_point)
                                     .SetTokenStart(current)),
                          std::nullopt, std::nullopt};
    Case('"') return {
        Str(state.value_of().DiscardPrefix().MovePositionBy(code_point).SetTokenStart(current)),
        std::nullopt, std::nullopt};
    Case(IsIdentFirst) return {Ident(state.value_of()
                                         .AddToPrefix(code_point)
                                         .MovePositionBy(code_point)
                                         .SetTokenStart(current)),
                               std::nullopt, std::nullopt};
    Case(kEofMarker) return {
        Eof(state.value_of().DiscardPrefix().MovePositionBy(code_point).SetTokenStart(current)),
        std::nullopt, std::nullopt};
    Otherwise() return {Whitespace(state.value_of().MovePositionBy(code_point)), std::nullopt,
                        std::format("Unknown symbol at ({}:{}): {}", current.line+1, current.column+1,
                                    ToString({code_point}))};
  }
  EndMatch throw std::logic_error{"unreachable"};
}

TokenizerOutput HandleState(char32_t code_point, const Str& state) {
  const auto [token_prefix, token_start, prev, current, ident_to_index, index_to_ident]
      = state.value_of();

  Match(code_point) {
    Case('\\') return {Escape(state.value_of().MovePositionBy(code_point)), std::nullopt,
                       std::nullopt};
    Case('"') return {Whitespace(state.value_of()
                                     .DiscardPrefix()
                                     .SetTokenStart(NextPosition(current, code_point))
                                     .MovePositionBy(code_point)),
                      StringLiteralToken(Coords{token_start, current}, ToString(token_prefix)),
                      std::nullopt};
    Case(kEofMarker) return {Eof(state.value_of()
                                     .DiscardPrefix()
                                     .SetTokenStart(current)
                                     .MovePositionBy(code_point)),
                      StringLiteralToken(Coords{token_start, prev}, ToString(token_prefix)),
                      std::format("Expected closing \" at ({}:{})", current.line+1, current.column+1)};
    Otherwise() return {Str(state.value_of().AddToPrefix(code_point).MovePositionBy(code_point)),
                        std::nullopt, std::nullopt};
  }
  EndMatch throw std::logic_error{"unreachable"};
}

TokenizerOutput HandleState(char32_t code_point, const Escape& state) {
  const auto [token_prefix, token_start, prev, current, ident_to_index, index_to_ident]
      = state.value_of();

  Match(code_point) {
    Case('n') return {Str(state.value_of().AddToPrefix('\n').MovePositionBy(code_point)),
                      std::nullopt, std::nullopt};
    Case('"') return {Str(state.value_of().AddToPrefix('\"').MovePositionBy(code_point)),
                                     std::nullopt, std::nullopt};
    Case('\\') return {Str(state.value_of().AddToPrefix('\\').MovePositionBy(code_point)),
                                     std::nullopt, std::nullopt};
    Case('t') return {Str(state.value_of().AddToPrefix('\t').MovePositionBy(code_point)),
                      std::nullopt, std::nullopt};
    Case(kEofMarker) return {
        Eof(state.value_of().DiscardPrefix().MovePositionBy(code_point).SetTokenStart(current)),
        StringLiteralToken(Coords{token_start, prev}, ToString(token_prefix)),
        std::format("Expected literal symbol and closing \" at ({}:{})", current.line+1,
                    current.column+1)};
    Otherwise() return {Str(state.value_of().AddToPrefix(code_point).MovePositionBy(code_point)),
                        std::nullopt,
                        std::format("Unknown escape sequence at ({}:{}): \\{}", current.line+1,
                                    current.column+1, ToString({code_point}))};
  }
  EndMatch throw std::logic_error{"unreachable"};
}

bool IsIdentFirstNotUnderscore(char32_t c) {
  return Any(c, IsAlpha, std::bind(Contains, U"$@", std::placeholders::_1));
}

TokenizerOutput HandleState(char32_t code_point, const Number& state) {
  const auto [token_prefix, token_start, prev, current, ident_to_index, index_to_ident]
      = state.value_of();

  Match(code_point) {
    Case(IsDigit) return {
        Number(state.value_of().AddToPrefix(code_point).MovePositionBy(code_point)),
        std::nullopt, std::nullopt};
    Case('_') return {
        Number(state.value_of().MovePositionBy(code_point)),
        std::nullopt, std::nullopt};
    Case(IsSpace) return {
        Whitespace(state.value_of().DiscardPrefix().MovePositionBy(code_point).SetTokenStart(current)),
        IntegerToken(Coords{token_start, prev}, std::stoll(ToString(token_prefix))), std::nullopt};
    Case(IsIdentFirstNotUnderscore) return {
        Ident(state.value_of().AddToPrefix(code_point).MovePositionBy(code_point)),
        IntegerToken(Coords{token_start, prev}, std::stoll(ToString(token_prefix))), std::nullopt};
    Case(kEofMarker) return {
        Eof(state.value_of().DiscardPrefix().MovePositionBy(code_point).SetTokenStart(current)),
        IntegerToken(Coords{token_start, prev}, std::stoll(ToString(token_prefix))), std::nullopt};
    Otherwise() return {Number(state.value_of().MovePositionBy(code_point)), std::nullopt,
                        std::format("Unknown symbol at ({}:{}): {}", current.line+1, current.column+1,
                                    ToString({code_point}))};
  }
  EndMatch throw std::logic_error{"unreachable"};
}

bool IsIdent(char32_t c) {
  return Any(c, IsAlpha, IsDigit, std::bind(Contains, U"@_$", std::placeholders::_1));
}

std::size_t GetIdentIndex(immer::map<std::string, std::size_t> ident_to_index, std::string ident) {
  const auto it = ident_to_index.find(ident);
  return it == nullptr ? ident_to_index.size() : *it;
}

TokenizerOutput HandleState(
    char32_t code_point, const Ident& state) {
  const auto [token_prefix, token_start, prev, current, ident_to_index, index_to_ident]
      = state.value_of();

  Match(code_point) {
    Case('"') return {Str(state.value_of()
                              .DiscardPrefix()
                              .SetTokenStart(current)
                              .MovePositionBy(code_point)
                              .AddIdentIfNotExists(ToString(token_prefix))),
                      IdentToken(Coords{token_start, prev},
                                 GetIdentIndex(ident_to_index, ToString(token_prefix))),
                      std::nullopt};
    Case(IsSpace) return {Whitespace(state.value_of()
                                         .DiscardPrefix()
                                         .SetTokenStart(current)
                                         .MovePositionBy(code_point)
                                         .AddIdentIfNotExists(ToString(token_prefix))),
                          IdentToken(Coords{token_start, prev},
                                     GetIdentIndex(ident_to_index, ToString(token_prefix))),
                          std::nullopt};
    Case(IsIdent) return {
        Ident(state.value_of().AddToPrefix(code_point).MovePositionBy(code_point)), std::nullopt,
        std::nullopt};
    Case(kEofMarker) return {Eof(state.value_of()
                                     .DiscardPrefix()
                                     .MovePositionBy(code_point)
                                     .SetTokenStart(current)
                                     .AddIdentIfNotExists(ToString(token_prefix))),
                             IdentToken(Coords{token_start, prev},
                                        GetIdentIndex(ident_to_index, ToString(token_prefix))),
                             std::nullopt};
    Otherwise() return {Ident(state.value_of().MovePositionBy(code_point)), std::nullopt,
                        std::format("Unknown symbol at ({}:{}): {}", prev.line+1, prev.column+1,
                                    ToString({code_point}))};
  }
  EndMatch throw std::logic_error{"unreachable"};
}

TokenizerOutput HandleState(char32_t code_point, const Eof& state) {
  throw std::logic_error{"HandleState called from EOF state"};
}

std::pair<char32_t, std::string> NextCodePoint(std::string s) {
  auto b = std::begin(s);
  auto e = std::end(s);

  const char32_t next = utf8::next(b, e);

  return {next, std::string(b, e)};
}

}  // namespace

TokenizerOutput Tokenize(
    char32_t code_point, TokenizerState state) {
  return std::visit([&code_point](const auto& state) { return HandleState(code_point, state); },
                    state);
}

std::tuple<TokenizerState, immer::flex_vector<Token>, std::optional<Message>> TokenizeWithEof(
  char32_t code_point, TokenizerState state) {
  Position pos = std::visit([](const auto& state_v) {
    return state_v.value_of().current;
  }, state);
  const auto [new_state, token, message] = Tokenize(code_point, state);
  if (code_point == kEofMarker && token.has_value()) {
    return {new_state, immer::flex_vector<Token>{token.value(), EofToken(Coords{pos, pos}, ' ')}, message};
  }
  if (code_point == kEofMarker) {
    return {new_state, immer::flex_vector<Token>{EofToken(Coords{pos, pos}, ' ')}, message};
  }
  if (token.has_value()) {
    return {new_state, immer::flex_vector<Token>{token.value()}, message};
  }
  return {new_state, {}, message};
}

TokenizerStringOutput Tokenize(std::string s, TokenizerState state) {
  return s.empty() ? std::make_tuple(state, Tokens{}, Messages{})
                   : [&state, &s] {
                       const auto [code_point, rest] = NextCodePoint(s);

                       const auto [next_state, token, message] = Tokenize(code_point, state);
                       const auto [res_state, tokens, messages] = Tokenize(rest, next_state);

                       const auto res_tokens = token.has_value() ? tokens.push_front(token.value()) : tokens;
                       const auto res_messages = message.has_value() ? messages.push_front(message.value()) : messages;
                       return std::make_tuple(res_state, res_tokens, res_messages);
                     }();
}

std::string GetName(const Whitespace&) {
  return "WS";
}

std::string GetName(const Str&) {
  return "STR";
}

std::string GetName(const Escape&) {
  return "ESCAPE";
}

std::string GetName(const Number&) {
  return "INTEGER";
}

std::string GetName(const Ident&) {
  return "IDENT";
}

std::string GetName(const Eof&) {
  return "EOF";
}

TokenizerStateData TokenizerStateData::DiscardPrefix() const {
  return TokenizerStateData{
      .token_prefix = {},
      .token_start = token_start,
      .prev = prev,
      .current = current,
      .ident_to_index = ident_to_index,
      .index_to_ident = index_to_ident,
  };
}

TokenizerStateData TokenizerStateData::AddToPrefix(char32_t c) const {
  return TokenizerStateData{
      .token_prefix = token_prefix.push_back(c),
      .token_start = token_start,
      .prev = prev,
      .current = current,
      .ident_to_index = ident_to_index,
      .index_to_ident = index_to_ident,
  };
}

TokenizerStateData TokenizerStateData::MovePositionBy(char32_t c) const {
  return TokenizerStateData{
      .token_prefix = token_prefix,
      .token_start = token_start,
      .prev = current,
      .current = NextPosition(current, c),
      .ident_to_index = ident_to_index,
      .index_to_ident = index_to_ident,
  };
}

TokenizerStateData TokenizerStateData::SetTokenStart(Position p) const {
  return TokenizerStateData{
      .token_prefix = token_prefix,
      .token_start = p,
      .prev = prev,
      .current = current,
      .ident_to_index = ident_to_index,
      .index_to_ident = index_to_ident,
  };
}

TokenizerStateData TokenizerStateData::AddIdentIfNotExists(std::string ident) const {
  return ident_to_index.find(ident) == nullptr ? TokenizerStateData{
      .token_prefix = token_prefix,
      .token_start = token_start,
      .prev = prev,
      .current = current,
      .ident_to_index = ident_to_index.set(ident, index_to_ident.size()),
      .index_to_ident = index_to_ident.push_back(ident),
  } : TokenizerStateData{
      .token_prefix = token_prefix,
      .token_start = token_start,
      .prev = prev,
      .current = current,
      .ident_to_index = ident_to_index,
      .index_to_ident = index_to_ident,
  };
}

std::string ToString(immer::flex_vector<char32_t> s) {
  std::string res;
  for (const auto& sym : s) {
    utf8::append(sym, res);
  }
  return res;
}

}  // namespace stewkk::lexer
