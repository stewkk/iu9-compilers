#pragma once

#include <iostream>
#include <ostream>

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL stewkk::lab11::Parser::symbol_type stewkk::lab11::Scanner::Get(stewkk::lab11::Driver& driver)

#include "location.h"
#include "parser.h"

namespace stewkk::lab11 {

class Driver;

class Scanner final : public yyFlexLexer {
 public:
  Scanner(std::istream& is = std::cin, std::ostream& os = std::cout,
          const std::string* isname = nullptr)
      : yyFlexLexer(is, os), loc_(isname) {}

  Parser::symbol_type Get(Driver& driver);

 private:
  location loc_;
};

}  // namespace stewkk::lab11
