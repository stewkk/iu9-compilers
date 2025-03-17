#pragma once

#include <cwctype>
#include <string>
#include <variant>
#include <tuple>

#include <immer/flex_vector.hpp>
#include <immer/map.hpp>
#include <strong_type/strong_type.hpp>

#include <stewkk/lexer/token.hpp>

namespace stewkk::lexer {

struct TokenizerStateData {
  immer::flex_vector<char32_t> token_prefix;
  Position token_start;
  Position prev;
  Position current;
  immer::map<std::string, std::size_t> ident_to_index;
  immer::flex_vector<std::string> index_to_ident;

  bool operator==(const TokenizerStateData& other) const = default;

  TokenizerStateData DiscardPrefix() const;
  TokenizerStateData AddToPrefix(char32_t c) const;
  TokenizerStateData MovePositionBy(char32_t c) const;
  TokenizerStateData SetTokenStart(Position p) const;
  TokenizerStateData AddIdentIfNotExists(std::string ident) const;
};

template <typename Tag> using StateType = strong::type<TokenizerStateData, Tag, strong::equality>;

using Whitespace = StateType<struct whitespace_>;
using Str = StateType<struct str_>;
using Escape = StateType<struct escape_>;
using Number = StateType<struct number_>;
using Ident = StateType<struct ident_>;
using Eof = StateType<struct eof_>;

using TokenizerState = std::variant<Whitespace, Str, Escape, Number, Ident, Eof>;

std::string GetName(TokenizerState state);

using Message = std::string;

using Tokens = immer::flex_vector<Token>;
using Messages = immer::flex_vector<Message>;

using TokenizerOutput = std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>>;
using TokenizerStringOutput = std::tuple<TokenizerState, Tokens, Messages>;

TokenizerOutput Tokenize(
    char32_t code_point, TokenizerState state);

TokenizerStringOutput Tokenize(std::string input, TokenizerState state);

std::string ToString(immer::flex_vector<char32_t> s);

constexpr const int kEofMarker = -1;

}  // namespace stewkk::lexer
