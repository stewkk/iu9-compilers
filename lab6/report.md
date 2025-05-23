% Лабораторная работа № 1.5 «Порождение лексического анализатора с помощью flex»
% 7 мая 2025 г.
% Александр Старовойтов, ИУ9-61Б

# Цель работы

Целью данной работы является изучение генератора лексических анализаторов flex.

# Индивидуальный вариант

- Комментарии: начинаются с «(*» или «{», заканчиваются на «*)» или «}» и могут
  пересекать границы строк текста.
- Целочисленные литералы: последовательности десятичных цифр.
- Дробные литералы: строки вида «digits/digits», где «digits» —
  последовательность десятичных цифр.

Атрибут (для лабораторных работ 1.3 и 1.5) дробного числа — пара целых чисел
(числитель и знаменатель).

# Реализация

```lex
%option noyywrap yylineno

%{

#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <format>
#include <sstream>
#include <ios>

using std::string_literals::operator""s;


struct Position {
    std::size_t line;
    std::size_t column;
};

struct Range {
    Position start;
    Position end;
};

struct Token {
    std::string text;
    Range coords;

    virtual std::string Attr() const = 0;
    virtual std::string Type() const = 0;
    virtual ~Token() = default;
};

struct Integer : public Token {
    std::int64_t value;

    std::string Attr() const override {
        return std::to_string(value);
    }

    std::string Type() const override {
        return "INTEGER"s;
    }
};

struct Fraction : public Token {
    std::int64_t numerator;
    std::int64_t denominator;

    std::string Attr() const override {
        return std::format("{}/{}", numerator, denominator);
    }

    std::string Type() const override {
        return "FRACTION"s;
    }
};

std::string MakeError(Position pos, std::string text) {
    return std::format("Error ({},{}): {}", pos.line, pos.column, std::move(text));
}

static std::vector<std::unique_ptr<Token>> tokens;
static std::vector<std::string> errors;
static std::vector<Range> comments;
static std::size_t start_column;
static std::size_t end_column = 1;
static std::size_t comment_line;
static std::size_t comment_column;

#define YY_USER_ACTION \
    { \
        start_column = end_column; \
        const std::string text = YYText(); \
        for (const auto ch : text) { \
            if (ch == '\n') { \
                end_column = 1; \
            } else { \
                end_column++; \
            } \
        } \
    }

%}

DIGIT [0-9]
INTEGER {DIGIT}+
FRACTION {INTEGER}\/{INTEGER}

%x IN_COMMENT IN_COMMENT_2

%%

[\t\n ]+
{INTEGER} {
            auto text = YYText();
            std::size_t line = lineno();
            try {
                std::int64_t number = std::stoll(text);

                auto token = std::make_unique<Integer>();
                token->text = std::move(text);
                token->coords = Range{
                    .start = Position {
                      .line = line,
                      .column = start_column,
                    },
                    .end = Position {
                      .line = line,
                      .column = end_column,
                    },
                };
                token->value = std::move(number);

                tokens.push_back(std::move(token));
            } catch (const std::exception& ex) {
                errors.push_back(
                MakeError(Position{line, end_column},
                std::format("{} can't fit in std::int64_t", text)));
            }
          }
{FRACTION} {
            auto text = YYText();
            std::size_t line = lineno();
            std::stringstream stream{text};
            stream.exceptions(std::ios_base::failbit|std::ios_base::badbit);
            try {
                std::int64_t numerator;
                std::int64_t denominator;
                stream >> numerator;
                stream.get();
                stream >> denominator;

                auto token = std::make_unique<Fraction>();
                token->text = std::move(text);
                token->coords = Range{
                    .start = Position {
                      .line = line,
                      .column = start_column,
                    },
                    .end = Position {
                      .line = line,
                      .column = end_column,
                    },
                };
                token->numerator = std::move(numerator);
                token->denominator = std::move(denominator);

                tokens.push_back(std::move(token));
            } catch (const std::exception& ex) {
                errors.push_back(MakeError(Position{line, end_column},
                std::format("{} can't fit in std::int64_t", text)));
            }
           }

"(*" {
          BEGIN(IN_COMMENT);
          comment_line = lineno();
          comment_column = start_column;
     }
<IN_COMMENT>{
"*)" {
         BEGIN(INITIAL);
         comments.push_back(Range{Position{comment_line, comment_column},
         Position{static_cast<std::size_t>(lineno()), end_column}});
    }
\n
.
<<EOF>> {
    errors.push_back(MakeError(Position{static_cast<std::size_t>(lineno()),
    end_column}, "EOF found, '*)' expected"));
    return 0;
}
}

"{" {
          BEGIN(IN_COMMENT_2);
          comment_line = lineno();
          comment_column = start_column;
     }
<IN_COMMENT_2>{
"}" {
         BEGIN(INITIAL);
         comments.push_back(Range{Position{comment_line, comment_column},
         Position{static_cast<std::size_t>(lineno()), end_column}});
    }
\n
.
<<EOF>> {
    errors.push_back(MakeError(Position{static_cast<std::size_t>(lineno()), end_column},
    "EOF found, '}' expected"));
    return 0;
}
}

. {
    errors.push_back(MakeError(Position{static_cast<std::size_t>(lineno()),
    start_column}, std::format("unexpected character: {}", YYText())));
}

%%

int main() {
    std::unique_ptr<FlexLexer> lexer = std::make_unique<yyFlexLexer>();
    while(lexer->yylex() != 0);

    for (auto& error : errors) {
        std::cout << error << std::endl;
    }

    for (auto& token : tokens) {
        std::cout << std::format("{} ({}, {})-({}, {}): {}\n",
                                 token->Type(),
                                 token->coords.start.line,
                                 token->coords.start.column,
                                 token->coords.end.line,
                                 token->coords.end.column,
                                 token->Attr());
    }

    for (auto& comment : comments) {
        std::cout << std::format("({}, {})-({}, {}) comment\n",
                                 comment.start.line,
                                 comment.start.column,
                                 comment.end.line,
                                 comment.end.column);
    }

    return 0;
}
```

# Тестирование

Входные данные

```
123 def (* 123 *)
{ } 1/3
1
```

Вывод на `stdout`

```
Error (1,5): unexpected character: d
Error (1,6): unexpected character: e
Error (1,7): unexpected character: f
INTEGER (1, 1)-(1, 4): 123
FRACTION (2, 5)-(2, 8): 1/3
INTEGER (3, 1)-(3, 2): 1
(1, 9)-(1, 18) comment
(2, 1)-(2, 4) comment
```

# Вывод

Изучил генератор лексических анализаторов flex.
