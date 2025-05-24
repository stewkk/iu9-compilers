#!/usr/bin/env python3

from lark import Lark, Token


DOMAINS = """
start: LB | RB | TERM | NONTERM
LB.1: "["
RB.1: "]"
COMMENT.1: /%.*/
NONTERM.1: /[A-Z]+'?/
TERM.0: /[^\\s]+/

%import common (WS)
%ignore COMMENT
%ignore WS
"""


def tokenize(text: str) -> list[Token]:
    return list(Lark(DOMAINS, lexer='basic').lex(text))
