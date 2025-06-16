#pragma once

#include <optional>

#include "ast.h"
#include "ident_table.h"
#include "scanner.h"

namespace stewkk::lab11 {

class Driver final {
  bool trace_scanning_, trace_parsing_;
  std::optional<Program> program_;
  IdentTable table_{};

 public:
  Driver(bool trace_scanning, bool trace_parsing);

  void Parse(const std::string& filename);

  void set_program(Program&& program) noexcept {
    program_ = std::move(program);
  }
  const Program& get_program() const {
    assert(program_.has_value());
    return program_.value();
  }

  IdentTable& get_ident_table() noexcept { return table_; }
};

}  // namespace stewkk::lab11
