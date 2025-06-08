#!/usr/bin/env python3

import lexer
from parser import *


TEXT = """
2 + 3
"""

TABLE = {'E': {'LCB': ['T', "E'"], 'NUM': ['T', "E'"]}, "E'": {'PLUS': ['PLUS', 'T', "E'"], '$': [], 'RCB': []}, 'T': {'LCB': ['F', "T'"], 'NUM': ['F', "T'"]}, "T'": {'STAR': ['STAR', 'F', "T'"], 'PLUS': [], '$': [], 'RCB': []}, 'F': {'NUM': ['NUM'], 'LCB': ['LCB', 'E', 'RCB']}}

def main():
    tokens = lexer.tokenize(TEXT)
    derivation_tree = top_down_parse(tokens, 'E', ['$', 'LB', 'RB', 'AXIOM', 'NONTERM', 'TERM', 'LCB', 'RCB', 'NUM', 'STAR', 'PLUS'], TABLE)
    print(get_dot(derivation_tree))


if __name__ == "__main__":
    main()
