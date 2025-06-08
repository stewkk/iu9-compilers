#!/usr/bin/env python3

import lexer
from lark import Token
from dataclasses import dataclass
from pprint import pprint
import uuid
from typing import Any
import copy


TEXT = """
2 + 3
"""

TABLE = {'E': {'LCB': ['T', "E'"], 'NUM': ['T', "E'"]}, "E'": {'PLUS': ['PLUS', 'T', "E'"], '$': [], 'RCB': []}, 'T': {'LCB': ['F', "T'"], 'NUM': ['F', "T'"]}, "T'": {'STAR': ['STAR', 'F', "T'"], 'PLUS': [], '$': [], 'RCB': []}, 'F': {'NUM': ['NUM'], 'LCB': ['LCB', 'E', 'RCB']}}



@dataclass
class Node:
    uuid: str
    name: str
    children: list['Node']
    attr: Any


def new_uuid() -> str:
    return "n"+str(uuid.uuid4()).replace('-', '')


def top_down_parse(tokens: list[Token], start: str, terminals: list[str], table: dict[str, dict[str, list[str]]]) -> Node:
    tokens.append(Token('$', '$'))
    derivation_tree: Node = Node(new_uuid(), start, list(), None)
    mag = [('$', None), (start, derivation_tree)]
    token = tokens[0]
    top = None
    while True:
        top, top_node = mag[-1]
        if top == '$':
            break
        if top in terminals:
            if top == token.type:
                top_node.children.append(Node(new_uuid(), f"{token} at {token.line}:{token.column}-{token.end_line}:{token.end_column}", list(), f'{token}'))
                mag.pop()
                tokens = tokens[1:]
                token = tokens[0]
            else:
                raise Exception(f'Error at {token.line}:{token.column}-{token.end_line}:{token.end_column}, {token}')
        elif top in table and token.type in table[top]:
            chain = table[top][token.type]
            chain = list(map(lambda t: (t, Node(new_uuid(), t, list(), None)), chain))
            for _, node in chain:
                top_node.children.append(node)
            if not chain:
                top_node.children.append(Node(new_uuid(), 'ε', list(), ""))
            mag.pop()
            mag += reversed(chain)
        else:
            raise Exception(f'Error at {token.line}:{token.column}-{token.end_line}:{token.end_column}, {token}')
    return derivation_tree


def get_dot(tree: Node) -> str:
    def get_dot_inner(node: Node) -> str:
        return f"{node.uuid} [label=\"{node.name}\"]\n" + \
            "".join(map(lambda child: f"{node.uuid} -> {child.uuid}\n", node.children)) + \
            (" { rank=same; " if len(node.children) > 1 else "") +\
            (" -> ".join(map(lambda child: f"{child.uuid}", node.children)) if len(node.children) > 1 else "")  + \
            (" [style=invis] }\n" if len(node.children) > 1 else "") + \
            "".join(map(get_dot_inner, node.children))

    return "digraph {\n" + \
        get_dot_inner(tree) + \
        "}\n"


def get_child(node: Node, name: str) -> Node|None:
    return next(filter(lambda node: node.name == name, node.children), None)


def set_axiom(root: Node) -> None:
    nt = get_child(root, "Nt")
    assert nt is not None
    token = get_child(nt, "NONTERM")
    global AXIOM
    assert token is not None
    AXIOM = token.children[0].attr


def handle_rule(root: Node) -> None:
    lhs = get_child(root, "Nt")
    assert lhs is not None

    lhs = get_child(lhs, "NONTERM")
    assert lhs is not None

    lhs = lhs.children[0].attr

    global RULES
    res_rhs = list()

    rhs = get_child(root, "Rhs")
    while rhs is not None and rhs.children[0].attr != "":
        productions = get_child(rhs, "Productions")
        assert productions is not None
        productions = get_child(productions, "ProductionsBody")

        res_productions = list()

        while productions is not None and productions.children[0].attr != "":
            term = get_child(productions, "TERM")
            nonterm = get_child(productions, "NONTERM")
            if term is not None:
                res_productions.append(term.children[0].attr)
            if nonterm is not None:
                res_productions.append(nonterm.children[0].attr)
            productions = get_child(productions, "ProductionsBody")

        res_rhs.append(res_productions)

        rhs = get_child(rhs, "RhsTail")

    RULES.append((lhs, res_rhs))


ACTIONS = {
    "Axiom": set_axiom,
    "Rule": handle_rule,
}
AXIOM = None
RULES = list()
FIRST = dict()


def dfs(tree: Node, level=0):
    if tree.name in ACTIONS:
        ACTIONS[tree.name](tree)
    print('  '*level+tree.name)
    for child in tree.children:
        dfs(child, level+1)


def is_nonterm(t):
    return t.isupper() or (t[:-1].isupper() and t[-1] == "'")


def first(rhs):
    if len(rhs) == 0:
        return {'ε'}
    if not is_nonterm(rhs[0]):
        return {rhs[0]}
    global FIRST
    f = FIRST[rhs[0]]
    if 'ε' not in f:
        return f
    return f.difference({'ε'}).union(first(rhs[1:]))


def calc_follow():
    follow = dict()
    for rule in RULES:
        lhs = rule[0]
        follow[lhs] = set()

    follow[AXIOM] = follow[AXIOM].union({'$'})

    for rule in RULES:
        lhs = rule[0]
        for rhs in rule[1]:
            for i, t in enumerate(rhs):
                if is_nonterm(t):
                    follow[t] = follow[t].union(first(rhs[i+1:]).difference({'ε'}))

    tmp = None
    while tmp != follow:
        tmp = copy.deepcopy(follow)
        for rule in RULES:
            lhs = rule[0]
            for rhs in rule[1]:
                if len(rhs) == 0:
                    continue
                if is_nonterm(rhs[-1]):
                    follow[rhs[-1]] = follow[rhs[-1]].union(follow[lhs])
                for i, t in enumerate(rhs[:-1]):
                    if is_nonterm(t) and 'ε' in first(rhs[i+1:]):
                        follow[t] = follow[t].union(follow[lhs])

    return follow


def calc_first():
    global FIRST

    for rule in RULES:
        lhs = rule[0]
        FIRST[lhs] = set()

    tmp = None
    while tmp != FIRST:
        tmp = copy.deepcopy(FIRST)
        for rule in RULES:
            lhs = rule[0]
            for rhs in rule[1]:
                FIRST[lhs] = FIRST[lhs].union(first(rhs))


def gen_table(tree: Node):
    dfs(tree)
    print(AXIOM)
    print(RULES)
    table = dict()

    calc_first()

    print(FIRST)
    follow = calc_follow()
    print(follow)

    for rule in RULES:
        lhs = rule[0]
        table[lhs] = dict()

    for rule in RULES:
        lhs = rule[0]
        for rhs in rule[1]:
            f = first(rhs)
            for a in f:
                if a == 'ε':
                    continue
                if a not in table[lhs]:
                    table[lhs][a] = list()
                if rhs in table[lhs][a]:
                    raise Exception("")
                table[lhs][a] = rhs
            if 'ε' in f:
                for b in follow[lhs]:
                    if b not in table[lhs]:
                        table[lhs][b] = list()
                    if rhs in table[lhs][b]:
                        raise Exception("alarm")
                    table[lhs][b] = rhs
    return table


def main():
    tokens = lexer.tokenize(TEXT)
    derivation_tree = top_down_parse(tokens, 'E', ['$', 'LB', 'RB', 'AXIOM', 'NONTERM', 'TERM', 'LCB', 'RCB', 'NUM', 'STAR', 'PLUS'], TABLE)
    print(get_dot(derivation_tree))
    # table = gen_table(derivation_tree)
    # res_table = dict()
    # for key, value in table.items():
    #     key = key[0]+key[1:].lower()
    #     res_table[key] = dict()
    #     for inner, inner_value in value.items():
    #         inner_array = copy.deepcopy(inner_value)
    #         inner_key = inner.upper()
    #         res = list()
    #         for el in inner_array:
    #             if el[0] == '"':
    #                 res.append(el[1:-1].upper())
    #             elif el.islower():
    #                 res.append(el.upper())
    #             else:
    #                 res.append(el[0]+el[1:].lower())
    #         res_table[key][inner_key] = res

    # print()
    # print(res_table)


if __name__ == "__main__":
    main()
