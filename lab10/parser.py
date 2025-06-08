#!/usr/bin/env python3

from lark import Token
from dataclasses import dataclass
import uuid
from typing import Any

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
