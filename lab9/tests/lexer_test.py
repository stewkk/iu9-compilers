#!/usr/bin/env python3

import pytest
import subprocess


@pytest.mark.parametrize("stdin, expected_stdout",
                         [
                             ("{ }",
                              'LCB (1:1-1:2): "{"\nRCB (1:3-1:4): "}"\n'),
                             ("'oaoa \"\t '  ><",
                              'STR (1:1-1:11): "\'oaoa "\t \'"\nGT (1:13-1:14): ">"\nLT (1:14-1:15): "<"\n'),
                             ("/* oaoa */ /**/ kek",
                              'IDENT (1:17-1:20): "kek"\n'),
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
                              """IDENT (2:1-2:9): "CartProd"
LCB (2:10-2:11): "{"
LB (3:3-3:4): "("
VAR (3:4-3:7): "t.X"
VAR (3:8-3:11): "e.X"
RB (3:11-3:12): ")"
LB (3:13-3:14): "("
VAR (3:14-3:17): "e.Y"
RB (3:17-3:18): ")"
EQ (3:19-3:20): "="
LT (3:21-3:22): "<"
IDENT (3:22-3:35): "CartProd-Bind"
VAR (3:36-3:39): "t.X"
VAR (3:40-3:43): "e.Y"
GT (3:43-3:44): ">"
LT (3:45-3:46): "<"
IDENT (3:46-3:54): "CartProd"
LB (3:55-3:56): "("
VAR (3:56-3:59): "e.X"
RB (3:59-3:60): ")"
LB (3:61-3:62): "("
VAR (3:62-3:65): "e.Y"
RB (3:65-3:66): ")"
GT (3:66-3:67): ">"
SEMICOLON (3:67-3:68): ";"
LB (4:3-4:4): "("
RB (4:20-4:21): ")"
LB (4:22-4:23): "("
VAR (4:23-4:26): "e.Y"
RB (4:26-4:27): ")"
EQ (4:28-4:29): "="
SEMICOLON (4:46-4:47): ";"
RCB (5:1-5:2): "}"
IDENT (7:1-7:14): "CartProd-Bind"
LCB (7:15-7:16): "{"
VAR (8:3-8:6): "t.X"
VAR (8:7-8:10): "t.Y"
VAR (8:11-8:14): "e.Y"
EQ (8:15-8:16): "="
LB (8:17-8:18): "("
VAR (8:18-8:21): "t.X"
VAR (8:22-8:25): "t.Y"
RB (8:25-8:26): ")"
LT (8:27-8:28): "<"
IDENT (8:28-8:41): "CartProd-Bind"
VAR (8:42-8:45): "t.X"
VAR (8:46-8:49): "e.Y"
GT (8:49-8:50): ">"
SEMICOLON (8:50-8:51): ";"
VAR (9:3-9:6): "t.X"
EQ (9:24-9:25): "="
SEMICOLON (9:42-9:43): ";"
RCB (10:1-10:2): "}"
IDENT (16:1-16:4): "Inc"
LCB (16:5-16:6): "{"
VAR (17:3-17:11): "e.Prefix"
VAR (17:12-17:18): "s.Last"
COMMA (17:18-17:19): ","
STR (17:20-17:32): "'0123456789'"
COLON (17:33-17:34): ":"
VAR (17:35-17:38): "e.1"
VAR (17:39-17:45): "s.Last"
VAR (17:46-17:52): "s.Next"
VAR (17:53-17:56): "e.2"
EQ (17:57-17:58): "="
VAR (17:59-17:67): "e.Prefix"
VAR (17:68-17:74): "s.Next"
SEMICOLON (17:74-17:75): ";"
VAR (18:3-18:11): "e.Prefix"
STR (18:12-18:15): "'9'"
EQ (18:16-18:17): "="
LT (18:18-18:19): "<"
IDENT (18:19-18:22): "Inc"
VAR (18:23-18:31): "e.Prefix"
GT (18:31-18:32): ">"
STR (18:33-18:36): "'0'"
SEMICOLON (18:36-18:37): ";"
EQ (19:20-19:21): "="
STR (19:22-19:25): "'1'"
SEMICOLON (19:25-19:26): ";"
RCB (20:1-20:2): "}"
IDENT (23:1-23:7): "NextId"
LCB (23:8-23:9): "{"
VAR (24:3-24:11): "e.prefix"
COMMA (25:5-25:6): ","
LT (25:7-25:8): "<"
IDENT (25:8-25:10): "Dg"
IDENT (25:11-25:18): "counter"
GT (25:18-25:19): ">"
COLON (26:5-26:6): ":"
LCB (26:7-26:8): "{"
VAR (27:9-27:12): "s.n"
EQ (27:13-27:14): "="
VAR (27:15-27:23): "e.prefix"
LT (27:24-27:25): "<"
IDENT (27:25-27:29): "Symb"
VAR (27:30-27:33): "s.n"
GT (27:33-27:34): ">"
LT (27:35-27:36): "<"
IDENT (27:36-27:38): "Br"
IDENT (27:39-27:46): "counter"
STR (27:47-27:50): "'='"
LT (27:51-27:52): "<"
IDENT (27:52-27:55): "Add"
NUMBER (27:56-27:57): "1"
VAR (27:58-27:61): "s.n"
GT (27:61-27:62): ">"
GT (27:62-27:63): ">"
SEMICOLON (27:63-27:64): ";"
EQ (28:26-28:27): "="
LT (28:28-28:29): "<"
IDENT (28:29-28:31): "Br"
IDENT (28:32-28:39): "counter"
STR (28:40-28:43): "'='"
NUMBER (28:44-28:45): "0"
GT (28:45-28:46): ">"
LT (28:47-28:48): "<"
IDENT (28:48-28:54): "NextId"
VAR (28:55-28:63): "e.prefix"
GT (28:63-28:64): ">"
SEMICOLON (28:64-28:65): ";"
RCB (29:7-29:8): "}"
SEMICOLON (29:8-29:9): ";"
RCB (30:1-30:2): "}"
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
