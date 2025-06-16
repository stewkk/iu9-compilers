%require "3.8.2"
%language "c++"
%skeleton "lalr1.cc"

%header
%locations

%define api.location.file "location.h"
%define api.namespace {stewkk::lab11}
%define api.parser.class {Parser}
%define api.token.constructor
%define api.token.prefix {TOKEN_}
%define api.token.raw
%define api.value.automove
%define api.value.type variant

%define parse.assert
%define parse.error detailed
%define parse.trace
%define parse.lac full

%parse-param {Scanner& scanner}

%param {Driver& driver}

%code requires {

#include "ast.h"

namespace stewkk::lab11 {

class Driver;
class Scanner;

}  // namespace stewkk::lab11

}

%code top {

#include <sstream>
#include <memory>

#include "driver.h"

#define yylex scanner.Get

}

%token
  <std::size_t>
    IDENT "identifier"
    NUMBER "number"

%token
  IF   "if"
  ELSE "else"
  INT  "int"
  THEN "then"
  NULL "null"
  CONS "cons"
  CAR  "car"
  CDR  "cdr"
  OR   "or"

  EQUALS      "="
  COMMA       ","
  SEMICOLON   ";"
  COLON ":"

  LEFT_PARENTHESIS     "("
  RIGHT_PARENTHESIS    ")"
  LEFT_SQUARE_BRACKET  "["
  RIGHT_SQUARE_BRACKET "]"

%left
  ADD_OP
  PLUS  "+"
  MINUS "-"
  
%left 
  MUL_OP
  STAR  "*"
  SLASH "/"

%precedence
  FUNC_CALL

%nterm
  <Func> func
  <std::vector<Func>> funcs
  <FuncType> func_type
  <FuncBody> func_body
  <std::vector<Arg>> args
  <Arg> arg
  
  <Type> type
  <std::vector<std::unique_ptr<Type>>>
    tuple_type_items
  <ElementaryType> elementary_type
  <ListType> list_type
  <TupleType> tuple_type

  <Statement> statement
  <IfStatement> if
  <BoolExpr> bool_expr
  <Expr> expr
  <Call> call
  <std::vector<Expr>> call_args
  <Callee> callee
  <std::vector<std::unique_ptr<Expr>>> list_elements
  <std::vector<std::unique_ptr<Expr>>> list_elements_tail

  <Ident> ident
  <Const> const
  <Op>
    add_op
    mul_op
  <Builtin>
    car_op
    cdr_op
    cons_op

%%

program:
  funcs
  {
    driver.set_program(std::move($1));
  }

funcs:
  funcs func
  {
    $$ = $1;
    $$.push_back($2);
  }
| %empty
  {
  }

func:
  IDENT func_type EQUALS func_body SEMICOLON
  {
    $$ = Func($1, $2, $4);
  }

func_type:
  "(" args ")" COLON type
  {
    $$ = FuncType($2, $5);
  }

args:
  args "," arg
  {
    $$ = $1;
    $$.push_back($3);
  }
| arg
  {
    $$.push_back($1);
  }

arg:
  IDENT ":" type
  {
    $$ = Arg($1, $3);
  }

type:
  elementary_type
  {
    $$ = $1;
  }
| list_type
  {
    $$ = $1;
  }
| tuple_type
  {
    $$ = $1;
  }

elementary_type:
  INT
  {
    $$ = ElementaryType(ElementaryType::Kind::kInt);
  }

list_type:
  "[" type "]"
  {
    $$ = ListType(std::make_unique<Type>($2));
  }

tuple_type:
  "(" tuple_type_items ")"
  {
    $$ = TupleType($2);
  }

tuple_type_items:
  type
  {
    $$.push_back(std::make_unique<Type>($1));
  }
| tuple_type_items "," type
  {
    $$ = $1;
    $$.push_back(std::make_unique<Type>($3));
  }

func_body:
  statement
  {
    $$ = FuncBody($1);
  }

statement:
  if
  {
    $$ = Statement($1);
  }
| expr
  {
    $$ = Statement($1);
  }

if:
  "if" bool_expr "then" expr "else" expr
  {
    $$ = IfStatement($2, $4, $6);
  }

bool_expr:
  bool_expr "or" bool_expr
  {
    $$ = OrExpr(std::make_unique<BoolExpr>($1), std::make_unique<BoolExpr>($3));
  }
| expr
  {
    $$ = $1;
  }
| "null" "(" expr ")"
  {
    $$ = NullExpr($3);
  }

expr:
  call
  {
    $$ = $1;
  }
| ident
  {
    $$ = $1;
  }
| const
  {
    $$ = $1;
  }
| expr add_op expr
  {
    $$ = BinaryExpr(std::make_unique<Expr>($1), std::make_unique<Expr>($3), $2);
  }
| expr mul_op expr
  {
    $$ = BinaryExpr(std::make_unique<Expr>($1), std::make_unique<Expr>($3), $2);
  }
| "[" list_elements "]"
  {
    $$ = ListExpr($2);
  }
| "(" list_elements ")"
  {
    $$ = TupleExpr($2);
  }

list_elements:
  list_elements_tail expr
  {
    $$ = $1;
    $$.push_back(std::make_unique<Expr>($2));
  }
| %empty
  {
  }

list_elements_tail:
  list_elements_tail expr ","
  {
    $$ = $1;
    $$.push_back(std::make_unique<Expr>($2));
  }
| %empty
  {
  }


call:
  callee "(" call_args ")"
  {
    $$ = Call($1, $3);
  }

call_args:
  call_args "," expr
  {
    $$ = $1;
    $$.push_back($3);
  }
| expr
  {
    $$.push_back($1);
  }

callee:
  ident
  {
    $$ = $1;
  }
| car_op
  {
    $$ = $1;
  }
| cdr_op
  {
    $$ = $1;
  }
| cons_op
  {
    $$ = $1;
  }

ident:
  IDENT
  {
    $$ = Ident($1);
  }

const:
  NUMBER
  {
    $$ = IntConst($1);
  }

car_op:
  CAR
  {
    $$ = Builtin::kCar;
  }

cdr_op:
  CDR
  {
    $$ = Builtin::kCdr;
  }

cons_op:
  CONS
  {
    $$ = Builtin::kCons;
  }

add_op:
  PLUS
  {
    $$ = Op::kAdd;
  }
| MINUS
  {
    $$ = Op::kSub;
  }

mul_op:
  STAR
  {
    $$ = Op::kMul;
  }
| SLASH
  {
    $$ = Op::kDiv;
  }

%%

namespace stewkk::lab11 {

void Parser::error(const location_type& loc, const std::string& msg) {
  throw syntax_error(loc, msg);
}

}  // namespace stewkk::lab11
