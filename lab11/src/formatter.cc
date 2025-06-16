#include "formatter.h"

namespace stewkk::lab11 {

Formatter::Formatter(const IdentTable& ident_table) : ident_table_(ident_table) {}

std::string Formatter::Format(const Program& program) {
    for (const auto& func : program) {
        Format(func);
        output_.ResetIdent();
        output_.NewLine();
        output_.NewLine();
    }
    return output_.GetStr();
}

void Formatter::Format(const Func& func) {
    const auto func_name = ident_table_.At(func.ident_code);
    output_.Put(func_name);
    if (output_.GetPrefixLength() + 1 > limit_) {
        output_.IncreaseIdent();
        output_.NewLine();
    }
    output_.Put("(");
    Format(func.type.args);
    output_.Put(")");

    output_.CheckpointLine();

    output_.Put(" : ");
    FormatSingleLine(func.type.result);
    output_.Put(" =");

    // TODO: если превысили длину префикса, восстанавливаемся и переносим на следующую строку

    Format(func.body);
    output_.Put(";");
}

void Formatter::Format(const FuncBody& body) {
    struct StatementFormatter {
        void operator()(const IfStatement& stmt) {
            output.IncreaseIdent();
            output.NewLine();
            output.Put("if ");
            auto if_ident = output.GetIdent();
            formatter.Format(stmt.condition);
            output.Put(" then");
            output.IncreaseIdent();
            output.NewLine();
            formatter.Format(stmt.then_expr);
            output.SetIdent(std::move(if_ident));
            output.NewLine();
            output.Put("else");
            output.IncreaseIdent();
            output.IncreaseIdent();
            output.NewLine();
            formatter.Format(stmt.else_expr);
        }
        void operator()(const Expr& stmt) {
            output.Put(" ");
            formatter.Format(stmt);
        }

        Formatter& formatter;
        Output& output;
    };

    std::visit(StatementFormatter{*this, output_}, body.statement);
}

void Formatter::Format(const Expr& expr) {
    struct ExprFormatter {
        void operator()(const Call& stmt) {
            formatter.Format(stmt.callee);
            output.Put("(");
            bool is_first = true;
            for (const auto& arg : stmt.args) {
                if (!is_first) {
                    output.Put(", ");
                }
                formatter.Format(arg);
                is_first = false;
            }
            output.Put(")");
        }
        void operator()(const Ident& stmt) {
            output.Put(formatter.ident_table_.At(stmt.code));
        }
        void operator()(const Const& stmt) {
            formatter.Format(stmt);
        }
        void operator()(const BinaryExpr& stmt) {
            formatter.Format(*stmt.lhs.get());
            output.Put(" ");
            output.Put(ToString(stmt.op));
            output.Put(" ");
            formatter.Format(*stmt.rhs.get());
        }
        void operator()(const ListExpr& stmt) {
            output.Put("[");
            bool is_first = true;
            for (const auto& elem : stmt.elements) {
                if (!is_first) {
                    output.Put(", ");
                }
                formatter.Format(*elem.get());
                is_first = false;
            }
            output.Put("]");
        }
        void operator()(const TupleExpr& stmt) {
            output.Put("(");
            bool is_first = true;
            for (const auto& elem : stmt.elements) {
                if (!is_first) {
                    output.Put(", ");
                }
                formatter.Format(*elem.get());
                is_first = false;
            }
            output.Put(")");
        }

        Formatter& formatter;
        Output& output;
    };

    std::visit(ExprFormatter{*this, output_}, expr);
}

void Formatter::Format(const BoolExpr& expr) {
    struct BoolExprFormatter {
        void operator()(const Expr& stmt) {
            formatter.Format(stmt);
        }
        void operator()(const OrExpr& stmt) {
            formatter.Format(*stmt.lhs.get());
            output.Put(" or ");
            formatter.Format(*stmt.rhs.get());
        }
        void operator()(const NullExpr& stmt) {
            output.Put("null(");
            formatter.Format(stmt.inner);
            output.Put(")");
        }

        Formatter& formatter;
        Output& output;
    };

    std::visit(BoolExprFormatter{*this, output_}, expr);
}

void Formatter::Format(const std::vector<Arg> &args) {
  bool is_first = true;
  for (const auto& arg : args) {
      if (!is_first) {
          output_.Put(", ");
      }
      const auto arg_name = ident_table_.At(arg.ident_code);
      output_.Put(arg_name);
      output_.Put(" : ");
      FormatSingleLine(arg.type);
      is_first = false;
  }
  // TODO: добавить форматирование каждого аргумента на отдельной строке
}

void Formatter::FormatSingleLine(const Type& type) {
    struct SingleLineFormatter {
        void operator()(const ElementaryType& type) {
            output.Put(ToString(type.kind));
        }
        void operator()(const ListType& type) {
            output.Put("[");
            formatter.FormatSingleLine(*type.type.get());
            output.Put("]");
        }
        void operator()(const TupleType& type) {
            output.Put("(");
            bool is_first = true;
            for (const auto& type : type.types) {
                if (!is_first) {
                    output.Put(", ");
                }
                formatter.FormatSingleLine(*type.get());
                is_first = false;
            }
            output.Put(")");
        }

        Formatter& formatter;
        Output& output;
    };

    std::visit(SingleLineFormatter{*this, output_}, type);
}

void Formatter::Format(const Const& expr) {
    struct ConstFormatter {
        void operator()(const IntConst& const_val) {
            output.Put(std::to_string(const_val.value));
        }

        Formatter& formatter;
        Output& output;
    };

    std::visit(ConstFormatter{*this, output_}, expr);
}

void Formatter::Format(const Callee& expr) {
    struct CalleeFormatter {
        void operator()(const Ident& ident) {
            output.Put(formatter.ident_table_.At(ident.code));
        }
        void operator()(const Builtin& builtin) {
            output.Put(ToString(builtin));
        }

        Formatter& formatter;
        Output& output;
    };

    std::visit(CalleeFormatter{*this, output_}, expr);
}

}  // namespace stewkk::lab11
