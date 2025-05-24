#!/usr/bin/env python3

import lexer
from lark import Token
from dataclasses import dataclass
from pprint import pprint


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


@dataclass
class Node:
    name: str
    children: list['Node']


def top_down_parse(tokens: list[Token], start: str, terminals: list[str], table: dict[str, dict[str, list[str]]]) -> Node:
    tokens.append(Token('$', '$'))
    res: Node = Node(start, list())
    mag = [('$', None), (start, res)]
    token = tokens[0]
    top = None
    while True:
        top, top_node = mag[-1]
        if top == '$':
            break
        if top in terminals:
            if top == token.type:
                mag.pop()
                tokens = tokens[1:]
                token = tokens[0]
            else:
                raise Exception(f'Error at {token.line}:{token.column}-{token.end_line}:{token.end_column}')
        elif top in table and token.type in table[top]:
            chain = table[top][token.type]
            chain = list(map(lambda t: (t, Node(t, list())), chain))
            for _, node in chain:
                top_node.children.append(node)
            mag.pop()
            mag += reversed(chain)
        else:
            raise Exception(f'Error at {token.line}:{token.column}-{token.end_line}:{token.end_column}')
    return res

def main():
    tokens = lexer.tokenize(TEXT)
    res = top_down_parse(tokens, 'Grammar', ['$', 'LB', 'RB', 'AXIOM', 'NONTERM', 'TERM'], TABLE)
    pprint(res)

if __name__ == "__main__":
    main()
