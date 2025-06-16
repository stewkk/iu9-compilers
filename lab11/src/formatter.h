#pragma once

#include <string>

#include "ast.h"
#include "ident_table.h"
#include "output.h"

namespace stewkk::lab11 {

class Formatter {
public:
    explicit Formatter(const IdentTable& ident_table);

    std::string Format(const Program& program);
private:
    void Format(const Func& func);
    void Format(const std::vector<Arg>& args);
    void Format(const FuncBody& body);
    void Format(const Expr& expr);
    void Format(const BoolExpr& expr);
    void Format(const Const& expr);
    void Format(const Callee& expr);

    void FormatSingleLine(const Type& type);

    Output output_;
    std::size_t limit_ = 80; // TODO: прокидывать через конструктор
    const IdentTable& ident_table_;
};

}  // namespace stewkk::lab11
