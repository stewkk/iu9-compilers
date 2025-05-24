#!/usr/bin/env python3

import lexer
from lark import Token
import copy


TEXT = """
% аксиома
[axiom [E]]
% правила грамматики
[E    [T E']]
[E'   [+ T E'] []]
[T    [F T']]
[T'   [* F T'] []]
[F    [n] [( E )]]
"""

TABLE = {
    'Grammar': {'LB': ['Axiom', 'Rules']},
    'Axiom': {'LB': ['LB', 'AXIOM', 'LB', 'Nt', 'RB', 'RB']},
    'Rules': {'LB': ['Rule', 'Rules'], '$': []},
    'Rule': {'LB': ['LB', 'Nt', 'Rhs', 'RB']},
    'Nt': {'NONTERM': ['NONTERM']},
    'Rhs': {'LB': ['Productions', 'RhsTail']},
    'RhsTail': {'LB': ['Productions', 'RhsTail'], 'RB': []},
    'Productions': {'LB': ['LB', 'ProductionsBody', 'RB']},
    'ProductionsBody': {'NONTERM': ['NONTERM', 'ProductionsBody'],
                        'TERM': ['TERM', 'ProductionsBody'],
                        'RB': []},
}


def top_down_parse(tokens: list[Token], start: str, terminals: list[str], table: dict[str, dict[str, list[str]]]):
    tokens.append(Token('$', '$'))
    res = []
    mag = ['$', start]
    token = tokens[0]
    top = None
    while True:
        top = mag[-1]
        if top == '$':
            break
        if top in terminals:
            if top == token.type:
                mag.pop()
                tokens = tokens[1:]
                token = tokens[0]
            else:
                raise Exception('one')
        elif top in table and token.type in table[top]:
            chain = table[top][token.type]
            mag.pop()
            mag += reversed(chain)
            res.append(f'{top} -> {chain}')
        else:
            raise Exception('other')
    return res

def main():
    tokens = lexer.tokenize(TEXT)
    res = top_down_parse(tokens, 'Grammar', ['$', 'LB', 'RB', 'AXIOM', 'NONTERM', 'TERM'], TABLE)
    print(res)

if __name__ == "__main__":
    main()
