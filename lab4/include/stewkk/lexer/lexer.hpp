#pragma once

#include <cwctype>
#include <string>
#include <variant>
#include <tuple>

#include <immer/flex_vector.hpp>
#include <strong_type/strong_type.hpp>

#include <stewkk/lexer/token.hpp>

namespace stewkk::lexer {

using Message = std::string;

struct TokenizerStateData {
  immer::flex_vector<char32_t> token_prefix;
  Position token_start;
  Position token_end;

  bool operator==(const TokenizerStateData& other) const = default;
};

template <typename Tag> using StateType = strong::type<TokenizerStateData, Tag, strong::equality>;

using Whitespace = StateType<struct whitespace_>;
using Str = StateType<struct str_>;
using Escape = StateType<struct escape_>;
using Number = StateType<struct number_>;
using Ident = StateType<struct ident_>;
using Eof = StateType<struct eof_>;

using TokenizerState = std::variant<Whitespace, Str, Escape, Number, Ident, Eof>;

using Tokens = immer::flex_vector<Token>;
using Messages = immer::flex_vector<Message>;

using TokenizerOutput = std::tuple<TokenizerState, std::optional<Token>, std::optional<Message>>;
using TokenizerStringOutput = std::tuple<TokenizerState, Tokens, Messages>;

TokenizerOutput Tokenize(
    char32_t code_point, const TokenizerState& state);

TokenizerStringOutput Tokenize(std::string input, const TokenizerState& state);

}  // namespace stewkk::lexer
