#!/usr/bin/env python3

from lark import Lark, Token


DOMAINS = """
start: LB | RB | TERM | NONTERM | AXIOM | LCB | RCB | NUM | STAR | PLUS

LB.1: "["
RB.1: "]"
LCB.1: "("
RCB.1: ")"
NUM.1: /[0-9]+/
STAR.1: "*"
PLUS.1: "+"
AXIOM.1: "axiom"
COMMENT.1: /%.*/
NONTERM.1: /[A-Z]+'?/
TERM.0: /[^\\s\\]\\[]+/

%import common (WS)
%ignore COMMENT
%ignore WS
"""


def tokenize(text: str) -> list[Token]:
    return list(Lark(DOMAINS, lexer='basic').lex(text))
