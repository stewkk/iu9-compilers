#!/usr/bin/env python3

import lexer
from parser import *

TEXT = """
2 + 3
"""

TABLE = {'E': {'LCB': ['T', "E'"], 'NUM': ['T', "E'"]}, "E'": {'PLUS': ['PLUS', 'T', "E'"], '$': [], 'RCB': []}, 'T': {'LCB': ['F', "T'"], 'NUM': ['F', "T'"]}, "T'": {'STAR': ['STAR', 'F', "T'"], 'PLUS': [], '$': [], 'RCB': []}, 'F': {'NUM': ['NUM'], 'LCB': ['LCB', 'E', 'RCB']}}


def dfs(root: Node):
    if root.attr is not None and root.attr != '':
        return [root.attr]
    attrs = list()
    for child in root.children:
        tmp = dfs(child)
        if tmp is not None:
            for el in tmp:
                attrs.append(el)
    print(attrs)
    if len(attrs) == 3:
        return calc(*attrs)
    return attrs


def calc(lhs, op, rhs):
    if op == "+":
        return int(lhs)+int(rhs)
    elif op == "*":
        return int(lhs)*int(rhs)
    raise Exception("unreachable")


def calc_expr(derivation_tree: Node):
    return dfs(derivation_tree)


def main():
    tokens = lexer.tokenize(TEXT)
    derivation_tree = top_down_parse(tokens, 'E', ['$', 'LB', 'RB', 'AXIOM', 'NONTERM', 'TERM', 'LCB', 'RCB', 'NUM', 'STAR', 'PLUS'], TABLE)
    print(get_dot(derivation_tree))
    print()
    print()
    print(calc_expr(derivation_tree))


if __name__ == "__main__":
    main()
