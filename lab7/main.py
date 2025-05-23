#!/usr/bin/env python3

import abc
import enum
import typing
from dataclasses import dataclass


class DefinitionBase(abc.ABC):
    pass


# Definition -> Struct | Enum | Union
@dataclass
class Definition:
    data: DefinitionBase


@dataclass
class NumVariablesDefinition:
    typename: str
    is_pointer: bool
    name: list[str]


# Struct -> STRUCT NameOpt StructFieldsOpt PointerOpt VariablesOpt ;
class Struct(DefinitionBase):
    name: str|None
    fields: list[Definition|NumVariablesDefinition]
    is_pointer: bool
    variables: list[str]


# Expr -> NUMBER | NAME | Expr + Expr | Expr - Expr | Expr * Expr | Expr / Expr | SIZEOF ( Expr )
class Expr(abc.ABC):
    pass


@dataclass
class ExprNumber(Expr):
    number: int


@dataclass
class ExprName(Expr):
    name: str


@dataclass
class BinOpExpr(Expr):
    lhs: Expr
    op: str
    rhs: Expr


@dataclass
class UnOpExpr(Expr):
    op: str
    expr: Expr


# EnumField -> NAME EnumFieldRhsOpt
class EnumField:
    name: str
    rhs: Expr|None


# Enum -> ENUM NameOpt EnumFieldsOpt PointerOpt VariablesOpt ;
class Enum(DefinitionBase):
    name: str|None
    fields: list[EnumField]
    is_pointer: bool
    variables: list[str]


# Union -> UNION NameOpt UnionFieldsOpt PointerOpt VariablesOpt ;
class Union(DefinitionBase):
    name: str|None
    fields: list[Definition|NumVariablesDefinition]
    is_pointer: bool
    variables: list[str]


# Program -> Definition Program | Definition
@dataclass
class Program:
    definitions: list[Definition]

