#!/usr/bin/env python3

from parser import Node, top_down_parse
import copy
import lexer

TEXT = """
[axiom [GRAMMAR]]
[GRAMMAR [AXIOM RULES]]
[AXIOM [lb "axiom" lb NT rb rb]]
[RULES [RULE RULES] []]
[RULE [lb NT RHS rb]]
[NT [nonterm]]
[RHS [PRODUCTIONS RHSTAIL]]
[RHSTAIL [PRODUCTIONS RHSTAIL] []]
[PRODUCTIONS [lb PRODUCTIONSBODY rb]]
[PRODUCTIONSBODY [term PRODUCTIONSBODY] [nonterm PRODUCTIONSBODY] []]
"""


TABLE = {'Grammar': {'LB': ['Axiom', 'Rules']},
          'Axiom': {'LB': ['LB', 'AXIOM', 'LB', 'Nt', 'RB', 'RB']},
          'Rules': {'LB': ['Rule', 'Rules'], '$': []},
          'Rule': {'LB': ['LB', 'Nt', 'Rhs', 'RB']},
          'Nt': {'NONTERM': ['NONTERM']},
          'Rhs': {'LB': ['Productions', 'Rhstail']},
          'Rhstail': {'LB': ['Productions', 'Rhstail'], 'RB': []},
          'Productions': {'LB': ['LB', 'Productionsbody', 'RB']},
          'Productionsbody': {'TERM': ['TERM', 'Productionsbody'],
                              'NONTERM': ['NONTERM', 'Productionsbody'],
                              'RB': []}}

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
        productions = get_child(productions, "Productionsbody")

        res_productions = list()

        while productions is not None and productions.children[0].attr != "":
            term = get_child(productions, "TERM")
            nonterm = get_child(productions, "NONTERM")
            if term is not None:
                res_productions.append(term.children[0].attr)
            if nonterm is not None:
                res_productions.append(nonterm.children[0].attr)
            productions = get_child(productions, "Productionsbody")

        res_rhs.append(res_productions)

        rhs = get_child(rhs, "Rhstail")

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
    derivation_tree = top_down_parse(tokens, 'Grammar',
                                     ['$', 'LB', 'RB', 'AXIOM', 'NONTERM', 'TERM', 'LCB', 'RCB', 'NUM', 'STAR', 'PLUS'],
                                     TABLE)
    table = gen_table(derivation_tree)
    res_table = dict()
    for key, value in table.items():
        key = key[0]+key[1:].lower()
        res_table[key] = dict()
        for inner, inner_value in value.items():
            inner_array = copy.deepcopy(inner_value)
            inner_key = inner.upper()
            res = list()
            for el in inner_array:
                if el[0] == '"':
                    res.append(el[1:-1].upper())
                elif el.islower():
                    res.append(el.upper())
                else:
                    res.append(el[0]+el[1:].lower())
            res_table[key][inner_key] = res

    print()
    print(res_table)


if __name__ == "__main__":
    main()
