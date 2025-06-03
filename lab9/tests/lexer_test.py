#!/usr/bin/env python3

import pytest
import subprocess


@pytest.mark.parametrize("stdin, expected_stdout",
                         [
                             ("{ }",
                              'LCB (1:1-1:1): "{"\nRCB (1:1-1:1): "}"\n'),
                             ("'oaoa \"\t '  ><",
                              'STR (1:1-1:1): "oaoa "\t "\nGT (1:1-1:1): ">"\nLT (1:1-1:1): "<"\n'),
                             ("/* oaoa */ /**/ kek",
                              'IDENT (1:1-1:1): "kek"\n'),
                             ("""
/* Декартово произведение */
CartProd {
  (t.X e.X) (e.Y) = <CartProd-Bind t.X e.Y> <CartProd (e.X) (e.Y)>;
  (/* пусто */) (e.Y) = /* пусто */;
}

CartProd-Bind {
  t.X t.Y e.Y = (t.X t.Y) <CartProd-Bind t.X e.Y>;
  t.X /* пусто */ = /* пусто */;
}

/*
  Прибавление 1 к строковой записи числа:
  123 → 124, 99 → 100, 007 → 008
*/
Inc {
  e.Prefix s.Last, '0123456789' : e.1 s.Last s.Next e.2 = e.Prefix s.Next;
  e.Prefix '9' = <Inc e.Prefix> '0';
  /* пусто */ = '1';
}

/* Функция возвращает уникальные идентификаторы с заданным префиксом */
NextId {
  e.prefix
    , <Dg counter>
    : {
        s.n = e.prefix <Symb s.n> <Br counter '=' <Add 1 s.n>>;
        /* пусто */ = <Br counter '=' 0> <NextId e.prefix>;
      };
}
                              """,
                              """IDENT (1:1-1:1): "CartProd"
LCB (1:1-1:1): "{"
LB (1:1-1:1): "("
VAR (1:1-1:1): "t.X"
VAR (1:1-1:1): "e.X"
RB (1:1-1:1): ")"
LB (1:1-1:1): "("
VAR (1:1-1:1): "e.Y"
RB (1:1-1:1): ")"
EQ (1:1-1:1): "="
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "CartProd-Bind"
VAR (1:1-1:1): "t.X"
VAR (1:1-1:1): "e.Y"
GT (1:1-1:1): ">"
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "CartProd"
LB (1:1-1:1): "("
VAR (1:1-1:1): "e.X"
RB (1:1-1:1): ")"
LB (1:1-1:1): "("
VAR (1:1-1:1): "e.Y"
RB (1:1-1:1): ")"
GT (1:1-1:1): ">"
SEMICOLON (1:1-1:1): ";"
LB (1:1-1:1): "("
RB (1:1-1:1): ")"
LB (1:1-1:1): "("
VAR (1:1-1:1): "e.Y"
RB (1:1-1:1): ")"
EQ (1:1-1:1): "="
SEMICOLON (1:1-1:1): ";"
RCB (1:1-1:1): "}"
IDENT (1:1-1:1): "CartProd-Bind"
LCB (1:1-1:1): "{"
VAR (1:1-1:1): "t.X"
VAR (1:1-1:1): "t.Y"
VAR (1:1-1:1): "e.Y"
EQ (1:1-1:1): "="
LB (1:1-1:1): "("
VAR (1:1-1:1): "t.X"
VAR (1:1-1:1): "t.Y"
RB (1:1-1:1): ")"
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "CartProd-Bind"
VAR (1:1-1:1): "t.X"
VAR (1:1-1:1): "e.Y"
GT (1:1-1:1): ">"
SEMICOLON (1:1-1:1): ";"
VAR (1:1-1:1): "t.X"
EQ (1:1-1:1): "="
SEMICOLON (1:1-1:1): ";"
RCB (1:1-1:1): "}"
IDENT (1:1-1:1): "Inc"
LCB (1:1-1:1): "{"
VAR (1:1-1:1): "e.Prefix"
VAR (1:1-1:1): "s.Last"
COMMA (1:1-1:1): ","
STR (1:1-1:1): "0123456789"
COLON (1:1-1:1): ":"
VAR (1:1-1:1): "e.1"
VAR (1:1-1:1): "s.Last"
VAR (1:1-1:1): "s.Next"
VAR (1:1-1:1): "e.2"
EQ (1:1-1:1): "="
VAR (1:1-1:1): "e.Prefix"
VAR (1:1-1:1): "s.Next"
SEMICOLON (1:1-1:1): ";"
VAR (1:1-1:1): "e.Prefix"
STR (1:1-1:1): "9"
EQ (1:1-1:1): "="
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "Inc"
VAR (1:1-1:1): "e.Prefix"
GT (1:1-1:1): ">"
STR (1:1-1:1): "0"
SEMICOLON (1:1-1:1): ";"
EQ (1:1-1:1): "="
STR (1:1-1:1): "1"
SEMICOLON (1:1-1:1): ";"
RCB (1:1-1:1): "}"
IDENT (1:1-1:1): "NextId"
LCB (1:1-1:1): "{"
VAR (1:1-1:1): "e.prefix"
COMMA (1:1-1:1): ","
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "Dg"
IDENT (1:1-1:1): "counter"
GT (1:1-1:1): ">"
COLON (1:1-1:1): ":"
LCB (1:1-1:1): "{"
VAR (1:1-1:1): "s.n"
EQ (1:1-1:1): "="
VAR (1:1-1:1): "e.prefix"
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "Symb"
VAR (1:1-1:1): "s.n"
GT (1:1-1:1): ">"
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "Br"
IDENT (1:1-1:1): "counter"
STR (1:1-1:1): "="
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "Add"
NUMBER (1:1-1:1): "1"
VAR (1:1-1:1): "s.n"
GT (1:1-1:1): ">"
GT (1:1-1:1): ">"
SEMICOLON (1:1-1:1): ";"
EQ (1:1-1:1): "="
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "Br"
IDENT (1:1-1:1): "counter"
STR (1:1-1:1): "="
NUMBER (1:1-1:1): "0"
GT (1:1-1:1): ">"
LT (1:1-1:1): "<"
IDENT (1:1-1:1): "NextId"
VAR (1:1-1:1): "e.prefix"
GT (1:1-1:1): ">"
SEMICOLON (1:1-1:1): ";"
RCB (1:1-1:1): "}"
SEMICOLON (1:1-1:1): ";"
RCB (1:1-1:1): "}"
"""),
                         ])
def test_lexer(stdin, expected_stdout):
    process = subprocess.run(
        [
            "refgo",
            "tests/lexer.rsl+lexer.rsl+LibraryEx.rsl",
        ],
        capture_output=True,
        text=True,
        input=stdin,
    )
    output = process.stdout, process.stderr

    assert output == (expected_stdout, "")
