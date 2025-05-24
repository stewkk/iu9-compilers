#!/usr/bin/env python3

import lexer


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


def main():
    tokens = lexer.tokenize(TEXT)


if __name__ == "__main__":
    main()
